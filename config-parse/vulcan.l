%option noyywrap

DIG=[0-9]
%x comm

%%

<comm>\([0-9],[0-9],[0-9],[0-9],[0-9],[0-9]\) {
                                 yytext[yyleng-3]=')';
                                 yytext[yyleng-2]=0;
                                 printf("%s\n", yytext);
                                 BEGIN(INITIAL);
                                }

^([0-9]+),1[^\(]* {BEGIN(comm);}

(.|\n)

%%

int main(){
  yylex();
}