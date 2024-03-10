#include <fcntl.h>
#include <glib-unix.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int input_fd;

// Ennek a függvénynek a típusa nem GSourceFunc, hanem GUnixFDSourceFunc
// (https://docs.gtk.org/glib/callback.UnixFDSourceFunc.html). Arról, hogy ez
// hogyan működik, lennt a g_source_set_callback-nél írtam.
gboolean adat_callback(gint fd, GIOCondition condition, gpointer user_data) {
  fprintf(stderr, "adat_callback: fd=%d, condition=%d, user_data=%p\n", fd, condition, user_data);

  // A G_IO_NVAL akkor van beállítva a condition-ben, amikor a file descriptor
  // amit megadtunk, hogy figyeljen, nincs is megnyitva. (Az NVAL az INVALID REQUEST
  // rövidítése.) A dokumentáció itt van: https://docs.gtk.org/glib/flags.IOCondition.html
  if (condition & G_IO_NVAL) {
    printf("Invalid request on fd=%d, stopping to listen\n", fd);
    return G_SOURCE_REMOVE;
  }

  // A condition bitjei között akkor lesz G_IO_HUP, ha a másik fél megszakította a
  // kapcsolatot ("hanged up", a telefon hasonlatáva élve). Ezt úgy lehet elérni,
  // ha például a "cat > be1"-et Ctrl-C-vel megszakítja az ember.
  if (condition & G_IO_HUP) {
    printf("Got hangup on fd=%d, stopping to listen\n", fd);
    return G_SOURCE_REMOVE;
  }

  // Fogalmam sincs, hogy hogyan lehet errort produkálni FIFO-val, lehet, hogy
  // nem mis lehet. Azt sem tudom egyelőre, hogy ha error van, akkor honnan 
  // lehet megtudni, hogy milyen error.
  if (condition & G_IO_ERR) {
    printf("Got error on fd=%d, stopping to listen\n", fd);
    return G_SOURCE_REMOVE;
  }

  static char buf[1024];
  int read_ret = read(input_fd, buf, 1023);
  if (read_ret == -1) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      return G_SOURCE_CONTINUE;
    }
    perror("reading");
    exit(1);
  }

  buf[read_ret] = '\0';
  printf("Incoming data from fd=%d: %s\n", fd, buf);

  return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[]) {
  input_fd = open(argv[1], O_RDONLY | O_NONBLOCK);
  if (input_fd == -1) {
    perror("opening the file");
    exit(1);
  }

  GMainContext *ctx = g_main_context_default();

  // Készítünk egy source-ot. A konstruktor második paraméterének a G_IO_IN-t
  // adjuk meg, aminek a hatása az, hogy az adott source akkor "tüzel", amikor
  // az input_fd-n adat jelent meg, amit ki lehet olvasni. A GLib titokban ehhez
  // hozzá teszi a G_IO_HUP-ot, a G_IO_ERR-t és a G_IO_NVAL-t is, tehát mintha
  // azt adtuk volna meg, hogy G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL.
  // Ennek a viselkedésnek nincsen dokumentációja (!), a forrásból kerestem ki
  // (csak érdekesség képpen, ebben a kódsorban találtam meg:
  // https://github.com/GNOME/glib/blob/a2021e4b26e003a123fd1b5c2809e494b08e0964/glib/gmain.c#L4005)
  // Ennek az a jelentősége, hogy a hibakezelés könnyebbé válik. Ha csak akkor
  // hívná meg a source callback függvényét, amikor jött adat, akkor olyan
  // esetekben, ha például bezárják a másik oldalt, nem tudná a program, hogy
  // mit csináljon.
  GSource *fsrc = g_unix_fd_source_new(input_fd, G_IO_IN);

  // Itt megadjuk, hogy milyen callback-et hívjon akkor, amikor az adott source
  // "tüzel".
  //
  // Van a GLib-ben egy nagy gányolás: azok a source-ok, amelyek unixos
  // file descriptort figyelnek, más típusú callback függvényt hívnak. A
  // híváskor egyszerűen átcastolja a rendszer a GSourceFunc-ot
  // GUnixFDSourceFunc-á. Erre elvileg a https://docs.gtk.org/glib/func.unix_fd_source_new.html
  // egyetlen sorából kellene, hogy rájöjjönk (Any callback attached to the
  // returned GSource must have type GUnixFDSourceFunc). Nekem ezt nem
  // sikerült észrevenni, és kétórás kódolvasással találtam meg a forráskódban
  // ezt a sort: https://github.com/GNOME/glib/blob/a2021e4b26e003a123fd1b5c2809e494b08e0964/glib/glib-unix.c#L300
  // Mivel a GSourceFunc típust nem lehet kiterjeszteni, és az API eléggé
  // elterjedt már, ezért ahelyett, hogy korrekt típusok lennének, elvárják
  // hogy hazudjon az ember, és GSourceFunc-cá konvertálja a saját GUnixFDSourceFunc-ját.
  //
  // A NULL-ok jelentését a g_source_set_callback leírásából tudod meg.
  // Az első azt jelenti, hogy az adat_callback-et a rendszer NULL user_data
  // paraméterrel hívja meg, a második NULL pedig azt jelenti, hogy nem kérünk
  // visszahívást akkor, ha magát a source-ot valamiért destroy-olja a rendszer.
  g_source_set_callback(fsrc, (GSourceFunc)adat_callback, NULL, NULL);
  
  // A g_source_attach dokumentációja azt mondja, hogy "Adds a GSource to
  // a context so that it will be executed within that context. Remove it
  // by calling g_source_destroy()." Ez sajnos nem elég egyértelmű: ez
  // jelentheti azt is, hogy kötelező remove-olni ahhoz, hogy clean shutdown
  // legyen, de jelentheti azt is, hogy ha már nem akarja az ember használni,
  // akkor így kell remove-olni. Ezt csak a glib forrásából lehet kideríteni.
  // Egyelőre különösebb aggodalom nélkül ignorálhatjuk a visszatérési értékét,
  // ami persze azt jelenti, hogy nem destroy-oljuk, de nem baj, a mi kis
  // példá programunknak ez biztos nem okoz komoly gondot. A tippem egyébként
  // az, hogy a rendszer magától remove-olja, tehát ez így rendben van.
  g_source_attach(fsrc, ctx);


  GMainLoop *main_loop = g_main_loop_new(ctx, TRUE);
  g_main_loop_run(main_loop);

  return 0;
}