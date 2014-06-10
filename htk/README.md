HTK

Step 1. check existing files.

a) data files (data/)
 - mfcc_trn.txt
 - mfcc_tst.txt
 - mfc/*

b) library files (lib/)
 - mono.txt : Dictionary (separated phones)
 - snor_trn.txt : training scripts
 - snor_tst.txt : testing scripts
 - snor.txt : all scripts
 - wpgram.txt : all words
 - sil.txt : silent words

Step 2. Create word-level label files (word)

script :

HDMan -A -D -V -T 1 -a '*;' -n lib/mono.list -g lib/mono.ded lib/mono.dct lib/mono.txt lib/sil.txt

grep -v '^;' lib/snor.txt | \
gawk 'BEGIN { printf("#\!MLF\!#\r\n"); } \
{ printf("%c*/%s.lab%c\r\n",34,tolower(substr($NF,2,length($NF)-3)),34); \
for (i=1;i<NF;i++) printf("%s\r\n",$i); printf(".\r\n"); }' > lib/snor.wordmlf

grep -v '^;' lib/snor_trn.txt | \
gawk 'BEGIN { printf("#\!MLF\!#\r\n"); } \
{ printf("%c*/%s.lab%c\r\n",34,tolower(substr($NF,2,length($NF)-3)),34); \
for (i=1;i<NF;i++) printf("%s\r\n",$i); printf(".\r\n"); }' > lib/snor_trn.wordmlf

grep -v '^;' lib/snor_tst.txt | \
gawk 'BEGIN { printf("#\!MLF\!#\r\n"); } \
{ printf("%c*/%s.lab%c\r\n",34,tolower(substr($NF,2,length($NF)-3)),34); \
for (i=1;i<NF;i++) printf("%s\r\n",$i); printf(".\r\n"); }' > lib/snor_tst.wordmlf
 
result : 
 - lib/snor.wordmlf
 - lib/snor_trn.wordmlf
 - lib/snor_tst.wordmlf
 
Step 3. Create mono.hled

text :
# lib/mono.hled
========================
EX
IS sil sil
DE sp
========================

Step 4. Create word-level label files (phone)

script :

HLEd -A -D -V -l '*' -i lib/snor_trn.phonemlf -d lib/mono.txt lib/mono.hled \
lib/snor_trn.wordmlf

HLEd -A -D -V -l '*' -i lib/snor_tst.phonemlf -d lib/mono.txt lib/mono.hled \
lib/snor_tst.wordmlf

HLEd -A -D -V -l '*' -i lib/snor.phonemlf -d lib/mono.txt lib/mono.hled \
lib/snor.wordmlf

Step 5. Create config file and proto file

text :
# lib/config.basic
========================
BYTEORDER       = VAX
SOURCEKIND      = WAVEFORM
SOURCEFORMAT    = NOHEAD
SOURCERATE      = 1250
ZMEANSOURCE     = FALSE

TARGETKIND      = MFCC_E_D_A
TARGETFORMAT    = HTK
TARGETRATE      = 100000
WINDOWSIZE      = 250000.0
NUMCHANS        = 24
ENORMALISE      = TRUE
NATURALREADORDER= T
========================

# work/proto
========================
~o
<STREAMINFO> 1 13
<VECSIZE> 13<MFCC_0>
~h "proto"
<BEGINHMM>
<NUMSTATES> 5
<STATE> 2
<MEAN> 13
0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
<VARIANCE> 13
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
<STATE> 3
<MEAN> 13
0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
<VARIANCE> 13
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
<STATE> 4
<MEAN> 13
0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
<VARIANCE> 13
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
<TRANSP> 5
0.0 1.0 0.0 0.0 0.0
0.0 0.5 0.5 0.0 0.0
0.0 0.0 0.5 0.5 0.0
0.0 0.0 0.0 0.5 0.5
0.0 0.0 0.0 0.0 0.0
<ENDHMM>

Sample Bytes:  2        Sample Kind:   WAVEFORM
  Num Comps:     1        Sample Period: 125.0 us
  Num Samples:   11680    File Format:   NOHEAD

========================
   
Step 6. Create Initial phone models

script : 
cd work

HCompV -A -D -V -C ../lib/config.basic -S mfcc_trn.txt -f 0.01 -m -M hmm0 proto

HERest -A -D -V -C ../lib/config.basic —T 1 -I ../lib/snor_trn.phonemlf -t 250.0 150.0 1000.0 \
-S mfcc_trn.txt -H hmm0/macros -H hmm0/hmmdefs -M hmm1 monophones0


Step . Grammar

# lib/gram.txt
==================
/*
 * Task grammar
 */

 $WORD = YES | NO;

 ( { START_SIL } [ $WORD ] { END_SIL } )
==================

HParse -A -D -T 1 ../lib/gram.txt net.slf

result
 - net.slf
