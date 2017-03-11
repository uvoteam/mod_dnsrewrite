
# mod_dnsrewrite

This is not an Apache module :) Not that it cannot be used with apache, though.
It just does to dns requests what mod_rewrite does to URLs.

This is a simple library, intended to be LD_PRELOADed with your program. It
reads mapping file, specified by environment variable DNSREWRITE_CONFIG and
substitutes names in getnameinfo() calls accordingly. This mapping file have a
very simple syntax - each line should contain two words, separated by whitespace
characters - what to search, and with what to substitute. Everything other is
ignored. Also you can add comments with hash sign (#). Like so:

~~~~
# redirect yandex to google
yandex.com       google.com
maps.yandex.com  maps.google.com

wikipedia.org    lurkmore.ru # booring
~~~~

That's it. For example, you can then run your program like so:

~~~~
LD_PRELOAD=/usr/lib/mod_dnsrewrite/dnsrewrite.so \
    DNSREWRITE_CONFIG=~/.dnsrewrite.conf myprog bla
~~~~

# building

To build this library you do not need anything special, except default system
headers and libraries. Just do:

~~~~
mkdir build
cd build
cmake ..
make
~~~~

If you wish - yo can finish with `sudo make install`.

  -- Mykhailo Danylenko <isbear@isbear.org.ua>
