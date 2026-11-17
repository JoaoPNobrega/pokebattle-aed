# Sprite original de Zapdos em pixel art para cobrir o asset ausente offline.
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
$raiz = Split-Path $PSScriptRoot -Parent
$destino = Join-Path $raiz 'assets/pokemon'
New-Item -ItemType Directory -Force $destino | Out-Null

$frente = @(
    '.......BB.......',
    '......YYYY......',
    '.B...BYYYYB...B.',
    'BB..YYYYYYYY..BB',
    '.BBYYYYYYYYYYBB.',
    '...YYYYYYYYYY...',
    '..BYYYYYYYYYYB..',
    '...YYYYWYYYYY...',
    '...YYYYYYYYYY...',
    '.BBYYYYYYYYYYBB.',
    'B..BYYYYYYYYB..B',
    '...YYYYYYYYYY...',
    '....Y..YY..Y....',
    '....Y..YY..Y....',
    '...OO..OO..OO...',
    '....GG...GG.....'
)
$costas = @(
    '.......BB.......',
    '......YYYY......',
    '.B...BYYYYB...B.',
    'BB..YYYYYYYY..BB',
    '.BBYYYYYYYYYYBB.',
    '...YYYYYYYYYY...',
    '..BYYYYYYYYYYB..',
    '...YYYYYYYYYY...',
    '...YYYYDYYYYY...',
    '.BBYYYYYYYYYYBB.',
    'B..BYYYYYYYYB..B',
    '...YYYYYYYYYY...',
    '....Y..YY..Y....',
    '....Y..YY..Y....',
    '...OO..OO..OO...',
    '....GG...GG.....'
)
$paleta = @{
    'Y' = [System.Drawing.Color]::FromArgb(255, 218, 48)
    'B' = [System.Drawing.Color]::FromArgb(40, 36, 52)
    'W' = [System.Drawing.Color]::FromArgb(255, 248, 216)
    'D' = [System.Drawing.Color]::FromArgb(200, 144, 32)
    'O' = [System.Drawing.Color]::FromArgb(232, 88, 32)
    'G' = [System.Drawing.Color]::FromArgb(88, 72, 64)
}

function Salvar-Sprite([string]$nome, [string[]]$linhas) {
    $imagem = New-Object System.Drawing.Bitmap(16, 16, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    for ($y = 0; $y -lt 16; $y++) {
        if ($linhas[$y].Length -ne 16) { throw "Linha $y do sprite $nome tem largura inválida." }
        for ($x = 0; $x -lt 16; $x++) {
            $pixel = $linhas[$y][$x]
            if ($pixel -ne '.') { $imagem.SetPixel($x, $y, $paleta[[string]$pixel]) }
        }
    }
    $imagem.Save((Join-Path $destino $nome), [System.Drawing.Imaging.ImageFormat]::Png)
    $imagem.Dispose()
}

Salvar-Sprite '145_front.png' $frente
Salvar-Sprite '145_back.png' $costas
Salvar-Sprite '145_icon.png' $frente
