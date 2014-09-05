QtExample
==========
This is a sample app making use of Qt Test framework in order to test some of its components.  This sample is meant to be usable as a starting point for anyone writing their app on how to test their features.

Things this code does:

* provides dummy sample for testing purposes:
   * The XML HTML Info example provides statistics about the content of HTML files.
   * The files are parsed using a QXmlStreamReader object. If the file does
     not contain a well-formed XML document, a description of the error is
     reported.
   * Note: This is the functionality it is testing.
* shows how to integrate and write tests using qttest. The required library('testlib') is already provided on the device.