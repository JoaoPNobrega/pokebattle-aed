# Ícones e guardas em pixel art
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
$raiz = Split-Path $PSScriptRoot -Parent
$pasta = Join-Path $raiz 'assets/ui'
New-Item -ItemType Directory -Force $pasta | Out-Null
$fontePrivada = New-Object System.Drawing.Text.PrivateFontCollection
$fontePrivada.AddFontFile((Join-Path $raiz 'assets/fontes/PressStart2P-Regular.ttf'))
$fonte = New-Object System.Drawing.Font($fontePrivada.Families[0], 8, [System.Drawing.FontStyle]::Regular, [System.Drawing.GraphicsUnit]::Pixel)
$atlas = New-Object System.Drawing.Bitmap(192, 16)
$grafico = [System.Drawing.Graphics]::FromImage($atlas)
$grafico.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::SingleBitPerPixelGridFit
$fundo = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(248, 240, 208))
$sombra = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(72, 64, 80))
$letras = @('Z','X','E','L','U','R','','','','','T','V')
for ($i = 0; $i -lt 12; $i++) {
    $x = $i * 16
    $grafico.FillRectangle($sombra, $x, 1, 16, 15)
    $grafico.FillRectangle($fundo, $x + 1, 1, 14, 12)
    $grafico.FillRectangle([System.Drawing.Brushes]::White, $x + 2, 2, 12, 1)
    if ($i -lt 6 -or $i -gt 9) {
        $grafico.DrawString($letras[$i], $fonte, [System.Drawing.Brushes]::Black, $x + 3, 3)
    } else {
        for ($j = 0; $j -lt 4; $j++) {
            if ($i -eq 6) { $grafico.FillRectangle([System.Drawing.Brushes]::Black, $x + 7 - $j, 3 + $j, 1 + 2 * $j, 1) }
            if ($i -eq 8) { $grafico.FillRectangle([System.Drawing.Brushes]::Black, $x + 4 + $j, 5 + $j, 7 - 2 * $j, 1) }
            if ($i -eq 7) { $grafico.FillRectangle([System.Drawing.Brushes]::Black, $x + 5 + $j, 3 + $j, 1, 7 - 2 * $j) }
            if ($i -eq 9) { $grafico.FillRectangle([System.Drawing.Brushes]::Black, $x + 9 - $j, 3 + $j, 1, 7 - 2 * $j) }
        }
    }
}
$atlas.Save((Join-Path $pasta 'teclas.png'), [System.Drawing.Imaging.ImageFormat]::Png)
$grafico.Dispose(); $atlas.Dispose(); $fonte.Dispose(); $fontePrivada.Dispose()
$guardas = New-Object System.Drawing.Bitmap(32, 24)
$grafico = [System.Drawing.Graphics]::FromImage($guardas)
$pele = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(224, 176, 128))
$uniformes = @([System.Drawing.Color]::FromArgb(160, 48, 64), [System.Drawing.Color]::FromArgb(72, 80, 160))
for ($i = 0; $i -lt 2; $i++) {
    $x = $i * 16
    $uniforme = New-Object System.Drawing.SolidBrush($uniformes[$i])
    $grafico.FillRectangle($sombra, $x + 4, 1, 8, 4)
    $grafico.FillRectangle($sombra, $x + 2, 4, 12, 2)
    $grafico.FillRectangle($pele, $x + 4, 6, 8, 6)
    $grafico.FillRectangle([System.Drawing.Brushes]::Black, $x + 5, 7, 2, 1)
    $grafico.FillRectangle([System.Drawing.Brushes]::Black, $x + 10, 7, 2, 1)
    $grafico.FillRectangle($uniforme, $x + 3, 12, 10, 7)
    $grafico.FillRectangle([System.Drawing.Brushes]::White, $x + 6, 13, 4, 3)
    $grafico.FillRectangle($uniforme, $x + 7, 14, 2, 1)
    $grafico.FillRectangle($pele, $x + 1, 13, 2, 5)
    $grafico.FillRectangle($pele, $x + 13, 13, 2, 5)
    $grafico.FillRectangle($sombra, $x + 4, 19, 3, 4)
    $grafico.FillRectangle($sombra, $x + 9, 19, 3, 4)
    $uniforme.Dispose()
}
$guardas.Save((Join-Path $pasta 'guardas.png'), [System.Drawing.Imaging.ImageFormat]::Png)
$grafico.Dispose(); $guardas.Dispose(); $fundo.Dispose(); $sombra.Dispose(); $pele.Dispose()
