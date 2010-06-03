#! /usr/bin/perl

$src = ".";
$src = $ARGV[0] if @ARGV == 1;

@languages = <$src/nls/*>;

s!^$src/nls/!!g for (@languages);

@languages = reverse sort @languages;
%lang2 = ();


for $lang (@languages)
{
   next if $lang !~ m/((\.dat))$/;

   $lang2 = $lang;
   $lang2 =~ s/[^a-zA-Z_0-9]/_/g;
   print "static String gv_nls_${lang2} [] = {\n";
   system "$src/ad2c nls/$lang";
   print "0};\n\n";
}

print "static String* getI18N(char* locale)\n{\n";
for $lang (@languages)
{
   next if $lang !~ m/((\.dat)|(\.lnk))$/;

   $lang2 = $lang;
   if ($lang =~ /\.lnk$/ )
   {
      open($file, "<", "$src/nls/$lang") or die "Cannot read file $lang";
      $lang2 = <$file>;
      chomp $lang2;
      close($lang);
   }
   $lang2 =~ s/[^a-zA-Z_0-9]/_/g;
   $lang3 = $lang;
   $lang3 =~ s/\.lnk$//g;
   $lang3 =~ s/\.dat$//g;
   print "   if (!strcmp(locale, \"$lang3\")) return gv_nls_$lang2;\n";
}
print "   return 0;\n}\n";
