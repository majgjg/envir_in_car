param(
    [string]$InputPath = "D:\sourcecode\envir_in_car\project\docs\thesis_draft.md",
    [string]$OutputPath = "C:\Users\25169\Desktop\abc\thesis_draft.docx"
)

$ErrorActionPreference = "Stop"

$WdStory = 6
$WdAlignParagraphLeft = 0
$WdAlignParagraphCenter = 1
$WdSaveFormatDocx = 16
$WdStyleNormal = -1
$WdStyleHeading1 = -2
$WdStyleHeading2 = -3
$WdStyleTitle = -63

function Resolve-AbsolutePath {
    param([string]$PathValue)
    $resolved = Resolve-Path -LiteralPath $PathValue
    return $resolved.Path
}

function Set-ParagraphFont {
    param(
        $Selection,
        [string]$AsciiFont,
        [string]$EastAsiaFont,
        [double]$Size,
        [bool]$Bold = $false,
        [bool]$Italic = $false
    )

    $Selection.Font.Name = $AsciiFont
    $Selection.Font.NameFarEast = $EastAsiaFont
    $Selection.Font.Size = $Size
    $Selection.Font.Bold = [int]$Bold
    $Selection.Font.Italic = [int]$Italic
}

function Add-TextParagraph {
    param(
        $Word,
        $Selection,
        [string]$Text,
        $StyleValue = $WdStyleNormal,
        [string]$AsciiFont = "Times New Roman",
        [string]$EastAsiaFont = "宋体",
        [double]$Size = 12,
        [bool]$Bold = $false,
        [bool]$Italic = $false,
        [int]$Alignment = $WdAlignParagraphLeft
    )

    $Selection.EndKey($WdStory) | Out-Null
    $Selection.Style = $StyleValue
    $Selection.ParagraphFormat.Alignment = $Alignment
    Set-ParagraphFont -Selection $Selection -AsciiFont $AsciiFont -EastAsiaFont $EastAsiaFont -Size $Size -Bold $Bold -Italic $Italic
    $Selection.TypeText($Text)
    $Selection.TypeParagraph()
}

function Add-CodeBlock {
    param($Word, $Selection, [string[]]$Lines)

    if (-not $Lines -or $Lines.Count -eq 0) {
        return
    }

    $Selection.EndKey($WdStory) | Out-Null
    $Selection.Style = $WdStyleNormal
    $Selection.ParagraphFormat.Alignment = $WdAlignParagraphLeft
    $Selection.ParagraphFormat.LeftIndent = 18
    $Selection.ParagraphFormat.RightIndent = 18
    Set-ParagraphFont -Selection $Selection -AsciiFont "Consolas" -EastAsiaFont "等线" -Size 10.5
    $Selection.TypeText(($Lines -join "`r"))
    $Selection.TypeParagraph()
    $Selection.ParagraphFormat.LeftIndent = 0
    $Selection.ParagraphFormat.RightIndent = 0
}

function Parse-TableRow {
    param([string]$Line)

    $cells = @()
    foreach ($cell in ($Line.Trim() -replace '^\|', '' -replace '\|$', '').Split('|')) {
        $cells += $cell.Trim()
    }
    return ,$cells
}

function Add-MarkdownTable {
    param($Word, $Document, $Selection, [string[]]$Lines)

    if (-not $Lines -or $Lines.Count -eq 0) {
        return
    }

    $dataRows = @()
    foreach ($line in $Lines) {
        if ($line -match '^\|\s*[-: ]+(\|\s*[-: ]+)+\|?\s*$') {
            continue
        }
        $dataRows += ,(Parse-TableRow -Line $line)
    }

    if ($dataRows.Count -eq 0) {
        return
    }

    $rowCount = $dataRows.Count
    $colCount = ($dataRows | ForEach-Object { $_.Count } | Measure-Object -Maximum).Maximum

    $Selection.EndKey($WdStory) | Out-Null
    $table = $Document.Tables.Add($Selection.Range, $rowCount, $colCount)
    $table.Borders.Enable = 1

    for ($r = 1; $r -le $rowCount; $r++) {
        for ($c = 1; $c -le $colCount; $c++) {
            $text = ""
            if ($c -le $dataRows[$r - 1].Count) {
                $text = $dataRows[$r - 1][$c - 1]
            }
            $table.Cell($r, $c).Range.Text = $text
            $table.Cell($r, $c).Range.Font.Name = "Times New Roman"
            $table.Cell($r, $c).Range.Font.NameFarEast = "宋体"
            $table.Cell($r, $c).Range.Font.Size = 10.5
            if ($r -eq 1) {
                $table.Cell($r, $c).Range.Font.Bold = 1
            }
        }
    }

    $Selection.SetRange($Document.Content.End - 1, $Document.Content.End - 1)
    $Selection.TypeParagraph()
}

$inputAbs = Resolve-AbsolutePath -PathValue $InputPath
$outputDir = Split-Path -Parent $OutputPath
if (-not (Test-Path -LiteralPath $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}

$lines = [System.IO.File]::ReadAllLines($inputAbs, [System.Text.Encoding]::UTF8)

$word = $null
$doc = $null

try {
    $word = New-Object -ComObject Word.Application
    $word.Visible = $false
    $word.DisplayAlerts = 0

    $doc = $word.Documents.Add()
    $sel = $word.Selection

    $doc.PageSetup.TopMargin = $word.CentimetersToPoints(2.5)
    $doc.PageSetup.BottomMargin = $word.CentimetersToPoints(2.5)
    $doc.PageSetup.LeftMargin = $word.CentimetersToPoints(3.0)
    $doc.PageSetup.RightMargin = $word.CentimetersToPoints(2.5)

    $inCode = $false
    $codeLines = New-Object System.Collections.Generic.List[string]
    $tableLines = New-Object System.Collections.Generic.List[string]

    foreach ($line in $lines) {
        if ($line -match '^```') {
            if ($tableLines.Count -gt 0) {
                Add-MarkdownTable -Word $word -Document $doc -Selection $sel -Lines $tableLines.ToArray()
                $tableLines.Clear()
            }

            if ($inCode) {
                Add-CodeBlock -Word $word -Selection $sel -Lines $codeLines.ToArray()
                $codeLines.Clear()
                $inCode = $false
            }
            else {
                $inCode = $true
            }
            continue
        }

        if ($inCode) {
            $codeLines.Add($line)
            continue
        }

        if ($line -match '^\|') {
            $tableLines.Add($line)
            continue
        }

        if ($tableLines.Count -gt 0) {
            Add-MarkdownTable -Word $word -Document $doc -Selection $sel -Lines $tableLines.ToArray()
            $tableLines.Clear()
        }

        if ([string]::IsNullOrWhiteSpace($line)) {
            continue
        }

        if ($line -match '^#\s+(.+)$') {
            Add-TextParagraph -Word $word -Selection $sel -Text $matches[1] -StyleValue $WdStyleTitle -EastAsiaFont "黑体" -Size 16 -Bold $true -Alignment $WdAlignParagraphCenter
            continue
        }

        if ($line -match '^##\s+(.+)$') {
            Add-TextParagraph -Word $word -Selection $sel -Text $matches[1] -StyleValue $WdStyleHeading1 -EastAsiaFont "黑体" -Size 14 -Bold $true
            continue
        }

        if ($line -match '^###\s+(.+)$') {
            Add-TextParagraph -Word $word -Selection $sel -Text $matches[1] -StyleValue $WdStyleHeading2 -EastAsiaFont "黑体" -Size 12 -Bold $true
            continue
        }

        if ($line -match '^>\s?(.*)$') {
            Add-TextParagraph -Word $word -Selection $sel -Text $matches[1] -StyleValue $WdStyleNormal -EastAsiaFont "楷体" -Size 11 -Italic $true
            continue
        }

        if ($line -match '^(\d+\.\s+.+)$') {
            Add-TextParagraph -Word $word -Selection $sel -Text $matches[1] -StyleValue $WdStyleNormal -Size 12
            continue
        }

        if ($line -match '^[-*]\s+(.+)$') {
            Add-TextParagraph -Word $word -Selection $sel -Text ("• " + $matches[1]) -StyleValue $WdStyleNormal -Size 12
            continue
        }

        Add-TextParagraph -Word $word -Selection $sel -Text $line -StyleValue $WdStyleNormal -Size 12
    }

    if ($tableLines.Count -gt 0) {
        Add-MarkdownTable -Word $word -Document $doc -Selection $sel -Lines $tableLines.ToArray()
        $tableLines.Clear()
    }

    if ($codeLines.Count -gt 0) {
        Add-CodeBlock -Word $word -Selection $sel -Lines $codeLines.ToArray()
        $codeLines.Clear()
    }

    $doc.SaveAs([ref]$OutputPath, [ref]$WdSaveFormatDocx)
    Write-Output "DOCX_CREATED: $OutputPath"
}
finally {
    if ($doc -ne $null) {
        $doc.Close()
    }
    if ($word -ne $null) {
        $word.Quit()
    }
}
