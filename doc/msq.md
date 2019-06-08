

comment = '%' LINE

score = ctrl* track+

track = _* NUM '|' _* events+

event = _* (duration? (rest | note | ctrl) | block ) ('*' NUM)? _ '&'?

ctrl = _* '<' '~'? _* ctrl args+ '>'

note = pitch octave?
rest = [R-][;]?
duration = 'z' | [bwhqest/.=]+(NUM ':' NUM)
pitch =  Z | [A-G][b#^_]* | [XT][+-]NUM 
octave = [0-9',]+

block = '(' (_* event )* _* ')'

macro = '$' ID ('=' block)?

setmark = '!:' NUM
gotomark = '!>' NUM



(qA & hC & hE ) & qR

(arp1)*2

$arp1=( t+0 t-2 /t+2 /t+3 )

zC5 $arp1 zE5 $arp1


@rand{{}
      {}
      {}}


qZ4 [A, =C E] hC, qR | q[]


 {3q(A C)}*2

 m$intro{}

q:3:2 A C E 


q<vol 70>  change volume from current value to 70 over a quarter note time

<cc to steps curve>

4b<vol 90>  <tempo 120> 


qA# hC qE

[qA4 hC5 qE5] hC4 qR

[:A maj:] [Eb min] [C sus2:1]

qZ0, hB# 


m$
1| qA  C  F
2|  