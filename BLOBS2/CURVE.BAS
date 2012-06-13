SCREEN 12

FOR x = 1 TO 640

PSET (639 - x, LOG((x * 160 / 640) ^ 2) * 480 / LOG(160 ^ 2))
NEXT
a$ = INPUT$(1)

