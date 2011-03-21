#! /usr/bin/perl

@files = ();

open (PIPE, "find ../../gv-i18n-admin -name .svn -prune -or -type f -print |");
while ($line = <PIPE>)
{
   chomp $line;
   push (@files, $line);
}
close (PIPE);

for $file (@files)
{
   system "cp $file ../gv/src/nls/";
   $file =~ m!/([^/]+)$!;
   $filename = $1;
   system "gv-update-userconfig -i ../gv/src/nls/$filename";
}
