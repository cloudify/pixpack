'
' qbasic program to generate a sin table
'

PI = 256                ' pi value in table
columns = 8             ' data columns

FOR i = 0 TO PI * 5 / 2 - 1
        IF (i MOD columns = 0) OR (i = 0) THEN PRINT : PRINT "DD ";  ELSE PRINT ",";
        fixed& = SIN(i * 3.14159 / PI) * 65536
        PRINT fixed&;
NEXT



