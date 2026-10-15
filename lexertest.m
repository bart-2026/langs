int lineno = 1
int colno = 0

const MAXTOKLEN = 1024

type tokentype = int

enumdata []ichar tknames =
    (TkERROR,   $),
    (TkEOF,     $),

    (TkSTAR,    $),
    (TkSLASH,   $),
    (TkPLUS,    $),
    (TkMINUS,   $),

    (TkLT,      $),
    (TkGT,      $),
    (TkEQ,      $),
    (TkBANG,    $),

    (TkGE,      $),
    (TkLE,      $),
    (TkSTRING,  $),
    (TkIDENT,   $),
end

record rtoken =
    tokentype tt
    ichar token
end

type itoken = ref rtoken

func newtoken(TokenType tt, ichar s, e)itoken =
    int length := rtoken.bytes + (e - s) + 1 + (e = s)
    itoken token := malloc(length)

    if token = nil then abortprogram("") fi
    memset(token, 0, length)

    token.tt := tt
    token.token := ichar(token) + rtoken.bytes
    token.token^ := tt
    if e<>s then
        memcpy(token.token, s, e - s)
    end
    token
end

func peek(filehandle fp)int ch =
    ch:=fgetc(fp)
    if ch<>c_eof then
        ungetc(ch, fp)
    end
    ch
end

func nexttoken(filehandle fp)itoken =
    [maxtoklen]char tokstr
    ichar tp := tokstr, te := &.tokstr + MAXTOKLEN - 1
    tokentype tt := TkEOF
    itoken next := nil
    int ch, c

    while (ch := fgetc(fp)) <> c_eof do
        tt := TkERROR
        ++colno

        switch ch
        when '\n' then
            ++lineno
            colno := 0

        when ' ', '\t', '\f', 14, '\r' then

        when '*' then
            tt := TkSTAR
            exit

        when '/' then
            tt := TkSLASH
            ch := peek(fp)

            switch ch
            when c_eof then
                exit
            when '/' then
                while ((ch := fgetc(fp)) <> '\n') do
                    exit all when ch = c_eof
                end
                ungetc(ch, fp)
                nextloop
            end
            exit

        when '+' then
            tt := TkPLUS
            exit

        when '-' then
            tt := TkMINUS
            exit

        when '<' then
            tt := TkLT
            tp++^ := '<' 
            ch := peek(fp)

            switch ch
            when c_eof then
                exit

            when '=' then
                tt := TkLE
                tp++^ := '='
            end

            tp++^ := '\0'
            exit

        when '>' then
            tt := TkGT
            tp++^ := '>'
            ch := peek(fp)

            switch ch
            when c_eof then
                exit

            when '=' then
                tt := TkGE
                tp++^ := '='
            end

            tp++^ := '\0'
            exit

        when '=' then
            tt := TkEQ
            exit

        when '!' then
            tt := TkBANG
            exit

        when '"' then
            tt := TkERROR

            while tp <> te do
                ch := fgetc(fp)
                switch ch
                when c_eof then
                    exit all
                when '"' then
                    exit

                when '\\' then
                    ch := fgetc(fp)

                    switch ch
                    when c_eof then
                        exit all
    
                    when 'n' then
                        ch := '\n'

                    when 'r' then
                        ch := '\r'
                    when 't' then
                        ch := '\t'
                    end
                end
                tp++^ := ch
            end
            tp++^ := '\0'
            tt := TkSTRING
            exit

        else
            c := ch
            exit when c <> '_' and  not isalnum(c)
            tp++^ := c
            while tp <> te do
                ch := fgetc(fp)
                exit all when c=c_eof
                c := ch

                if c<>'_' and not isalnum(c) then
                    ungetc(ch, fp)
                    exit
                end

                tp++^ := c
            end

            tp++^ := '\0'
            tt := TkIDENT
            exit
        end
    end

    newtoken(tt, tokstr, tp)
end


proc main =
    itoken tok
    filehandle stdin:=os_getstdin()

    while tok := nexttoken(stdin) do
        exit when tok.tt = TkEOF

        if tok.tt <> tkERROR then
            fprintln "tok: tt = #, token = '#'", tknames[tok.tt], tok.token
        end

        free(tok)
    end
end
