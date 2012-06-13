'
' qbasic program to generate an acos table
'

columns = 8             ' data columns

FOR i = 0 TO 512
        IF (i MOD columns = 0) OR (i = 0) THEN PRINT "    ; "; i - 1: PRINT "DW ";  ELSE PRINT ",";
        x = (i / 256 - 1)
        IF x = 1 THEN
                acos = 0
        ELSEIF x = -1 THEN
                acos = 3.14159
        ELSE
                acos = -ATN(x / SQR(-x * x + 1)) + 1.5708
        END IF
        fixed& = 400 * (acos / 3.14159 - .5)
        IF fixed& < -128 THEN fixed& = -128
        IF fixed& > 127 THEN fixed& = 127
        fixed& = fixed& + 128
        PRINT fixed&;
NEXT



