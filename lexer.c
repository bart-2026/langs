#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t lineno = 1;
static size_t colno = 0;

#define	MAXTOKLEN	1024

typedef enum TokenType TokenType;
enum TokenType {
	TkERROR = -1,
	TkEOF = 0,

	TkSTAR = '*',
	TkSLASH = '/',
	TkPLUS = '+',
	TkMINUS = '-',

	TkLT = '<',
	TkGT = '>',
	TkEQ = '=',
	TkBANG = '!',

	TkGE = 0x1000,
	TkLE,
	TkSTRING,
	TkIDENT,
	// etc....
};

typedef struct Token Token;
struct Token {
	TokenType type;
	char *token;
};

Token *
newtoken(TokenType tt, char *s, char *e)
{
	size_t len = sizeof(Token) + (e - s) + 1 + (e == s);
	Token *token = malloc(len);
	if (token == NULL) abort();
	memset(token, 0, len);
	token->type = tt;
	token->token = (char *)token + sizeof(*token);
	token->token[0] = tt;
	if (e != s)
		memcpy(token->token, s, e - s);
	return token;
}

static int
peek(FILE *fp)
{
	int ch = fgetc(fp);
	if (ch != EOF)
		ungetc(ch, fp);
	return ch;
}

Token *
nexttoken(FILE *fp)
{
	char tokstr[MAXTOKLEN], *tp = tokstr, *te = tokstr + MAXTOKLEN - 1;
	enum TokenType tt = TkEOF;
	Token *next = NULL;
	int ch;
	unsigned char c;

	loop:
	while ((ch = fgetc(fp)) != EOF) {
		tt = TkERROR;
		++colno;
		switch (ch) {
		case '\n':
			++lineno;
			colno = 0;
		case ' ':
		case '\t':
		case '\f':
		case '\v':
		case '\r':
			break;
		case '*':
			tt = TkSTAR;
			break loop;
		case '/':
			tt = TkSLASH;
			ch = peek(fp);
			switch (ch) {
			case EOF:
				break loop;
			case '/':
				while ((ch = fgetc(fp)) != '\n')
					if (ch == EOF)
						break loop;
				ungetc(ch, fp);
				continue;
			default:
				break;
			}
			break loop;
		case '+':
			tt = TkPLUS;
			break loop;
		case '-':
			tt = TkMINUS;
			break loop;
		case '<':
			tt = TkLT;
			*tp++ = '<'; 
			ch = peek(fp);
			switch (ch) {
			case EOF:
				break loop;
			case '=':
				tt = TkLE;
				*tp++ = '=';
				break;
			default:
				break;
			}
			*tp++ = '\0';
			break loop;
		case '>':
			tt = TkGT;
			*tp++ = '>';
			ch = peek(fp);
			switch (ch) {
			case EOF:
				break loop;
			case '=':
				tt = TkGE;
				*tp++ = '=';
				break;
			default:
				break;
			}
			*tp++ = '\0';
			break loop;
		case '=':
			tt = TkEQ;
			break loop;
		case '!':
			tt = TkBANG;
			break loop;
		case '"':
			tt = TkERROR;
			sloop:
			while (tp != te) {
				ch = fgetc(fp);
				switch (ch) {
				case EOF:
					break loop;
				case '"':
					break sloop;

				case '\\':
					ch = fgetc(fp);
					switch (ch) {
					case EOF:
						break loop;
					case 'n':
						ch = '\n';
						break;
					case 'r':
						ch = '\r';
						break;
					case 't':
						ch = '\t';
						break;
					default:
						break;
					}
				default:
					*tp++ = ch;
					break;
				}
			}
			*tp++ = '\0';
			tt = TkSTRING;
			break loop;
		default:
			c = ch;
			if (c != '_' && !isalnum(c))
				break loop;
			*tp++ = c;
			while (tp != te) {
				ch = fgetc(fp);
				if (ch == EOF)
					break loop;
				c = ch;
				if (c != '_' && !isalnum(c)) {
					ungetc(ch, fp);
					break;
				}
				*tp++ = c;
			}
			*tp++ = '\0';
			tt = TkIDENT;
			break loop;
		}
	}
	next = newtoken(tt, tokstr, tp);
	return next;
}

int
main(void)
{
	Token *tok;
	while ((tok = nexttoken(stdin)) != NULL) {
		if (tok->type == TkEOF)
			break;
		if (tok->type != TkERROR)
			printf("tok: tt = %x, token = '%s'\n", tok->type, tok->token);
		free(tok);
	}
	return 0;
}
