# Find all the MessagingTestNumNum files and run them

$files = Get-ChildItem -Path .\ -Filter MessagingTest*.exe -Recurse

foreach ($file in $files) {
    Write-Host "Running " $file.FullName
    $output = & $file.FullName | Out-String
    Write-Host $output.Trim()
} 
