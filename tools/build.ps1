param(
# Build mode
[ValidateSet("Debug", "Release")]
[string]
$Mode
)

$sep = [string]::new('-', 35)
$projRoot = Resolve-Path "$PSScriptRoot/.."
$buildDir = Join-Path $projRoot "build"

if (!(Test-Path $buildDir)) { mkdir $buildDir }

Push-Location $buildDir
conan install .. -s build_type=$Mode
echo "conan returned $LASTEXITCODE"
cmake --build . --config $Mode
echo "cmake returned $LASTEXITCODE"
Pop-Location
