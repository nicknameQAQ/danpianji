param([string]$OutputDirectory)

$ErrorActionPreference = 'Stop'
$source = $PSScriptRoot
if (-not $OutputDirectory) {
    $OutputDirectory = Join-Path (Split-Path -Parent $source) '单片机资料完整副本'
}
$destination = [System.IO.Path]::GetFullPath($OutputDirectory)
if (Test-Path -LiteralPath $destination) {
    throw "目标目录已存在，请指定一个不存在的目录：$destination"
}
[System.IO.Directory]::CreateDirectory($destination) | Out-Null

$names = @('单片机课程设计', '单片机课程设计2', '单片机课程设计3', '单片机设计实例350例')
foreach ($name in $names) {
    Copy-Item -LiteralPath (Join-Path $source $name) -Destination (Join-Path $destination $name) -Recurse
}

$restored = 0
foreach ($row in Import-Csv -LiteralPath (Join-Path $source '重复文件对照.csv')) {
    $keptPath = Join-Path $destination $row.KeptPath.Replace('/', '\')
    $omittedPath = Join-Path $destination $row.OmittedPath.Replace('/', '\')
    [System.IO.Directory]::CreateDirectory((Split-Path -Parent $omittedPath)) | Out-Null
    [System.IO.File]::Copy($keptPath, $omittedPath, $false)
    $restored++
}

Write-Output "已在 $destination 恢复 $restored 个重复文件。"
