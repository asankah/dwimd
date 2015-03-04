# Introduction #

This tool is for diagnosing issues with Windows Attachment Manager that may be preventing you from successfully downloading. It runs through some of the same checks as ` IAttachmentExecute::Save() ` method [described here](http://msdn.microsoft.com/en-us/library/windows/desktop/bb776299(v=vs.85).aspx) and provides more verbose results.

# Getting #

The tool can be downloaded from the [downloads page](http://code.google.com/p/dwimd/downloads/list).

# Running #

For a basic invocation, simply execute the ` dwimd.exe ` tool:

```
C:\path\to\dwimd> dwimd.exe

Test parameters:
  path = C:\Users\username\Downloads\test_download_389238.zip
  url  = http://www.example.com/download

Running test: AV Scanners

  Running test with {2781761E-28E1-4109-99FE-B9D127C57AFE}
      Name: Microsoft Antimalware IOfficeAntiVirus implementation
      Path: c:\Program Files (x86)\Microsoft Security Client\Antimalware\MpOAv.dll
    Scanner returns 0 (S_OK. The scan was successful.)
    Time elapsed: 31ms

Running test: Attachment Policy
    Invoking Attachment Services
    IAttachmentExecute::CheckPolicy() returned code 1 (S_FALSE. Prompt)
    IAttachmentExecute::Save() returned code 0 (S_OK. The scan was successful.)
    Time elapsed: 62ms
  Done

Summary:
No failures detected.
```

The output contains a summary that will contain details if any issues were found.

# Command line options #

Usage: ` diwmd [-p <path>|-f <filename>] [-u <url>] [-n] [-v] `

  * **-p=**_` < `path` > `_ or **--path**=_` < `path` > `_ : Set the path to the downloaded file. Dwimd creates a temporary test file at this location. If a file already exists, it will not be overwritten.

> If the path isn't specified, the user's downloads directory will be used. The name of the test file will be as described in the ` -f ` option below.

  * **-f=**_` < `filename` > `_ or **--filename**=_` < `filename` > `_: Set the name of the temporary test file. If a filename isn't specified, a random filename will be used.

  * **-u=**_` < `url` > `_ or **--url**=_` < `url` > `_: Set the source URL for the download. The URL is used to determine whether the download came from a trusted source. The default URL if one isn't specified is ` http://www.example.com/download `.

  * **-n**: Prevent the creation of a temporary file. If not specified, ` dwimd ` will create a temporary file in the default location or the location specified by the ` -p ` option with a filename as described in the ` -f ` option. If ` -n ` is specified, ` dwimd ` assumes that the test should use the currently existing file at this location.

> Warning: Since ` dwimd ` invokes ` IAttachmentExecute::Save() `, if any of the tests fail, the file may be deleted.

  * **-v**: Enable verbose logging.

# Interpreting Results #

## The ` AV Scanners ` Test ##

This test runs through all the scanners that are registered and enabled for the current user that implements the [IOfficeAntivirus](http://msdn.microsoft.com/en-us/library/office/ff830310.aspx) interface. These may be invoked by ` IAttachmentExecute::Scan() `.

The ` AV Scanners ` test results will look like the following:

```
Running test: AV Scanners

  Running test with {2781761E-28E1-4109-99FE-B9D127C57AFE}
      Name: Microsoft Antimalware IOfficeAntiVirus implementation
      Path: c:\Program Files (x86)\Microsoft Security Client\Antimalware\MpOAv.dll
    Scanner returns 0 (S_OK. The scan was successful.)
    Time elapsed: 31ms
```

The GUID that is displayed in the first line is the CLSID of the implementation. Other details like the name and path are displayed below that.

If the scanner returns a failure code, ` IAttachmentExecute::Scan() ` may fail and cause the download to be deleted. If the scanner or the corresponding AV product is misconfigured, it may be possible that the results are unexpected or incorrect.

The expected results from a scan are described in the documentation for [IOfficeAntivirus::Scan()](http://msdn.microsoft.com/en-us/library/office/ff830313.aspx). Any other return code likely indicates a problem with the AV product.

## The ` Attachment Policy ` test ##

This test invokes the ` CheckPolicy() ` and ` Save() ` methods of ` IAttachmentExecute `. The results look like the following:

```
Running test: Attachment Policy
    Invoking Attachment Services
    IAttachmentExecute::CheckPolicy() returned code 1 (S_FALSE. Prompt)
    IAttachmentExecute::Save() returned code 0 (S_OK. The scan was successful.)
    Time elapsed: 62ms
  Done
```

The valid return values for the ` CheckPolicy ` call are documented [here](http://msdn.microsoft.com/en-us/library/windows/desktop/bb776294(v=vs.85).aspx). In general, if the policy check returns a code other than ` S_OK ` or ` S_FALSE `, then the attachment should be considered blocked by security policy.

If this test fails, check your security zone settings. If the security zone for the source URL is configured to disallow downloads, for example, your downloads may be getting blocked.

# References #

  * [KB 883260](http://support.microsoft.com/kb/883260) Description of how the Attachment Manager works in Microsoft Windows.

  * [KB 815141](http://support.microsoft.com/kb/Q815141) Internet Explorer Enhanced Security Configuration changes the browsing experience

