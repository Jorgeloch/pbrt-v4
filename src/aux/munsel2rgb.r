library(aqp)

munsell_colors <- c(
  "5YR 6/1",      # Cinza
  "10YR 8/1",     # Branco
  "7.5YR 8/6",    # Amarelo/Bege
  "5YR 7/6",      # Laranja
  "2.5YR 5/8",    # Vermelho
  "10YR 4/6",     # Marrom
  "2.5YR 2.5/1"   # Preto
)

rgb_colors <- parseMunsell(munsell_colors)
print(rgb_colors)
