Attribute VB_Name = "Module1"
Public CalcState As Long
Public EventState As Boolean
Public PageBreakState As Boolean

Public sRows
Public max_id

Public Const DCount As Integer = 13

' Arrays
Dim Rules(1000)
Dim Ids(1000)
Dim Idu(1000)
Dim Idu2(1000)
Dim Idn(1000)

' Export
Dim eSpec(1000)
Dim eVoices(1000)
Dim eSeverity(1000)
Dim eRule(1000)
Dim eSubRule(1000)
Dim eComment(1000)
Dim eGComment(1000)
Dim eFlag(1000)
Dim eGroup(1000)
Dim eClass(1000)
Dim eData(DCount, 1000)

Sub OptimizeCode_Begin()

Application.ScreenUpdating = False

EventState = Application.EnableEvents
Application.EnableEvents = False

CalcState = Application.Calculation
Application.Calculation = xlCalculationManual

PageBreakState = ActiveSheet.DisplayPageBreaks
ActiveSheet.DisplayPageBreaks = False

Application.DisplayStatusBar = False
Application.DisplayAlerts = False
End Sub

Sub OptimizeCode_End()

ActiveSheet.DisplayPageBreaks = PageBreakState
Application.Calculation = CalcState
Application.EnableEvents = EventState
Application.ScreenUpdating = True
Application.DisplayStatusBar = True
Application.DisplayAlerts = True

End Sub

Sub LoadData()
  
  cID = 1
  cClass = 2
  cGroup = 3
  cRule = 4
  cSubRule = 5
  
  cData = 6
  cSev = cData + DCount
  cFlag = cData + DCount + 1
  cGCom = cData + DCount + 2
  cCom = cData + DCount + 3
  cSet = cData + DCount + 4
  
  Erase Idu2
  
  LastRow = Sheets("Data").Cells.Find("*", searchorder:=xlByRows, searchdirection:=xlPrevious).Row
  LastCol = Sheets("Data").Cells.Find("*", searchorder:=xlByColumns, searchdirection:=xlPrevious).Column
  
  Rng1 = Sheets("Data").Range(Cells(1, 1).Address, Cells(LastRow, cData - 1).Address).Value
  Rng2 = Sheets("Data").Range(Cells(1, cData).Address, Cells(LastRow, cData + DCount).Address).Value
  Rng3 = Sheets("Data").Range(Cells(1, cData + DCount).Address, Cells(LastRow, LastCol).Address).Value
  ' Process line by line and replace names
  For L = 2 To LastRow
    rid = Rng1(L, cID)
    If Not IsNumeric(rid) Then
      MsgBox ("Found non-numeric ID " + ID + " in row " + CStr(L))
      Exit Sub
    End If
    srow = Idn(rid)
    ' Report rules that were not found in main table
    If srow = 0 Then
      MsgBox ("Rule " + CStr(rid) + " is not found in main table")
      Exit Sub
    Else
      Rng3(L, 1) = eSeverity(srow)
      Rng1(L, cClass) = eClass(srow)
      Rng1(L, cGroup) = eGroup(srow)
      Rng1(L, cRule) = eRule(srow)
      Rng1(L, cSubRule) = eSubRule(srow)
      Rng3(L, 2) = eFlag(srow)
      Rng3(L, 3) = eSpec(srow)
      Rng3(L, 4) = eVoices(srow)
      Rng3(L, 5) = eGComment(srow)
      Rng3(L, 6) = eComment(srow)
    End If
    ' Load data for csv export
    For d = 1 To DCount
      eData(d, rid) = Rng2(L, d)
    Next d
    ' Check for duplicates
    If Idu2(rid) <> 0 Then
      MsgBox ("Rule " + CStr(rid) + " is found two times at row " + CStr(Idu2(rid)) + " and " + CStr(L))
      Exit Sub
    End If
    Idu2(rid) = L
  Next L
  Sheets("Data").Range(Cells(1, 1).Address, Cells(LastRow, cData - 1).Address).Value = Rng1
  Sheets("Data").Range(Cells(1, cData + DCount).Address, Cells(LastRow, LastCol).Address).Value = Rng3
  ' Add rules that were not found in report to bottom
  For rid = 1 To max_id + 1
    If Idu(rid) <> 0 And Idu2(rid) = 0 Then
      srow = Idn(rid)
      LastRow = LastRow + 1
      Sheets("Data").Cells(LastRow, cSet).Value = 1
      Sheets("Data").Cells(LastRow, cID).Value = rid
    End If
  Next rid
End Sub

Sub Compile()
Attribute Compile.VB_ProcData.VB_Invoke_Func = "d\n14"
  Call ExportVbaCode
  If TestFiltered = 1 Then Exit Sub
  Call OptimizeCode_Begin
  ' First line to search eRule
  First_line = 2
  First_col = 5
  Strict_col = 13
  ' Column number of first id and comment (zero-based)
  Id_column = 0
  Com_column = 0
  max_subrules = 8
  ' Set variables
  path = ActiveWorkbook.path
  ' Open file
  Dim fso As Object
  Set fso = CreateObject("Scripting.FileSystemObject")
  Dim oFile As Object
  Set oFile = fso.CreateTextFile(path + "\rules.csv")
  ' Write header
  oFile.WriteLine "Set;Rid;Species;Severity;Class;Group;Rule;Subrule;Accept;Comment;Subcomment;Viztype;VI;V2;VT;FQ;CF;GF;IF;DEU;MEI;ER;UE;R"
  Sheets("Missing").UsedRange.ClearContents
  Sheets("Missing").Range("A1").Value = "Rid"
  'Sheets("Data").UsedRange.ClearContents
  'Sheets("Data").Range("A1").Value = "Set"
  'Sheets("Data").Range("B1").Value = "Rid"
  'Sheets("Data").Range("C1").Value = "Severity"
  'Sheets("Data").Range("D1").Value = "Class"
  'Sheets("Data").Range("E1").Value = "Group"
  'Sheets("Data").Range("F1").Value = "Rule"
  'Sheets("Data").Range("G1").Value = "Subrule"
  'Sheets("Data").Range("H1").Value = "Flag"
  'Sheets("Data").Range("I1").Value = "GComment"
  'Sheets("Data").Range("J1").Value = "Comment"
  cst = ""
  gst = ""
  rline = 1
  sRows_old = 0
  ssent = 0
  max_id = 0
  ' Cycle through sheets
  scount = Worksheets.count
  For s = 1 To scount
    Set ws = Worksheets(s)
    ' Cycle through eRule
    If IsNumeric(ws.Name) Then
      ' Find IDs column
      Set idc = ws.Range("E1:CC1").Find("IDs", LookIn:=xlValues, LookAt:=xlPart, MatchCase:=False)
      Id_column = idc.Column - 1
      Erase Idu
      Erase Idn
      ' Find eComment column
      Set comc = ws.Range("E1:CC1").Find("Comment", LookIn:=xlValues, LookAt:=xlPart, MatchCase:=False)
      Com_column = comc.Column
      sRows = 0
      ssent = ssent + 1
      ' Write strict rule
      myComment = ws.Cells(1, Strict_col).Comment.Text
      myComment = Replace(myComment, vbCr, " ")
      myComment = Replace(myComment, vbLf, " ")
      If ws.Cells(1, Strict_col).Font.Bold Then
        oFile.WriteLine CStr(ws.Name) + ";0;;0;;;Strict;;0;" + myComment + ";;;;;;;;;;;;;;"
      Else
        oFile.WriteLine CStr(ws.Name) + ";0;;0;;;Strict;;1;" + myComment + ";;;;;;;;;;;;;;"
      End If
      For Line = First_line To 1000
        rule = ws.Cells(Line, 3).Value
        spec = ws.Cells(Line, 4).Value
        voices = ws.Cells(Line, 5).Value
        If rule = "" Then Exit For
        rule = Replace(rule, ";", ",")
        ' Get class and group
        If ws.Cells(Line, 1) <> "" Then cst = ws.Cells(Line, 1)
        If ws.Cells(Line, 2) <> "" Then gst = ws.Cells(Line, 2)
        ldiv = 0
        ' Cycle through eRule
        For div = 1 To max_subrules
          ' Get link to division
          Set dc = ws.Cells(Line, First_col + div)
          dst = dc.Value
          col = dc.Interior.Color
          If dst = "" Then
            GoTo NextDiv
          End If
          ldiv = ldiv + 1
          sRows = sRows + 1
          rline = rline + 1
          ' Get id
          rid = ws.Cells(Line, Id_column + div)
          If rid = 0 Then
            MsgBox ("Rule in sheet " + CStr(ws.Name) + " line " + CStr(Line) + " subrule " + CStr(div) + " has zero id: " + _
              vbCrLf + frule)
            Exit Sub
          End If
          dst = Replace(dst, ";", ",")
          ' Calculate flag
          flag = 1
          If dc.Font.Bold Then
            flag = 0
          End If
          If dc.Font.Size > 11 Then flag = 2
          If dc.Font.Strikethrough Then flag = -1
          sev = Round(((col And 255) - (col \ 256 And 255)) / 2 + 50)
          If sev < 0 Then sev = 0
          If sev > 100 Then sev = 100
          frule = rule + " - " + CStr(dst)
          eGComment(sRows) = ws.Cells(Line, Com_column)
          eComment(sRows) = ""
          If Not (dc.Comment Is Nothing) Then eComment(sRows) = dc.Comment.Text
          eComment(sRows) = Replace(eComment(sRows), vbCr, " ")
          eComment(sRows) = Replace(eComment(sRows), vbLf, " ")
          ' Check that sheets match
          If ssent > 1 Then
            If Rules(sRows) <> frule Then
              MsgBox ("Rule in sheet " + CStr(ws.Name) + " does not correspond with previous sheet: " + _
                vbCrLf + frule + vbCrLf + Rules(sRows))
              Exit Sub
            End If
            If eSeverity(sRows) <> sev Then
              MsgBox ("Rule in sheet " + CStr(ws.Name) + " has different color with previous sheet: " + _
                vbCrLf + frule + vbCrLf + CStr(sev) + vbCrLf + CStr(eSeverity(sRows)))
              Exit Sub
            End If
            If Ids(sRows) <> rid Then
              MsgBox ("Rule in sheet " + CStr(ws.Name) + " has different id with previous sheet: " + _
                vbCrLf + frule + vbCrLf + CStr(rid) + vbCrLf + CStr(Ids(sRows)))
              Exit Sub
            End If
            ' Set comment
            'dc.ClearComments
            'If eComment(sRows) <> "" Then
            '  dc.AddComment eComment(sRows)
            'End If
          Else
            Rules(sRows) = frule
            eRule(sRows) = rule
            eSubRule(sRows) = dst
            eSeverity(sRows) = sev
            eSpec(sRows) = spec
            eVoices(sRows) = voices
            eFlag(sRows) = flag
            Ids(sRows) = rid
            eClass(sRows) = cst
            eGroup(sRows) = gst
          End If
          ' Highlight duplicate id
          'If Idu(rid) Then
          '  ws.Cells(Line, Id_column + div).Font.Color = RGB(255, 0, 0)
          'Else
          '  ws.Cells(Line, Id_column + div).Font.Color = RGB(0, 0, 0)
          'End If
          Idu(rid) = Idu(rid) + 1
          Idn(rid) = sRows
          If rid > max_id Then max_id = rid
NextDiv:
        Next div
      Next Line
      If sRows_old > 0 And sRows <> sRows_old Then
        MsgBox ("Total Subrule count in sheet " + CStr(ws.Name) + " is " + CStr(sRows) + _
          ", which is different from previous sheet: " + CStr(sRows_old))
        Exit Sub
      End If
      sRows_old = sRows
    End If
  Next s
  Call LoadData
  For srow = 1 To sRows
    rid = Ids(srow)
    ' Write rules
    st = "1;" + CStr(rid) + ";" + CStr(eSpec(srow)) + ";" + CStr(eVoices(srow)) + ";" + CStr(eSeverity(srow)) + ";" + CStr(eClass(srow)) + _
      ";" + CStr(eGroup(srow)) + ";" + CStr(eRule(srow)) + _
      ";" + CStr(eSubRule(srow)) + ";" + CStr(eFlag(srow)) + ";" + _
      CStr(eGComment(srow)) + ";" + CStr(eComment(srow))
    For d = 1 To DCount
      st = st + ";" + CStr(eData(d, rid))
    Next d
    oFile.WriteLine st
    ' Write report
    'Sheets("Data").Cells(rline, 1).Value = ws.Name
    'Sheets("Data").Cells(rline, 2).Value = rid
    'Sheets("Data").Cells(rline, 3).Value = sev
    'Sheets("Data").Cells(rline, 4).Value = cst
    'Sheets("Data").Cells(rline, 5).Value = gst
    'Sheets("Data").Cells(rline, 6).Value = rule
    'Sheets("Data").Cells(rline, 7).Value = dst
    'Sheets("Data").Cells(rline, 8).Value = flag
    'Sheets("Data").Cells(rline, 9).Value = eGComment(sRows)
    'Sheets("Data").Cells(rline, 10).Value = eComment(sRows)
  Next srow
  oFile.Close
  Set fso = Nothing
  Set oFile = Nothing
  ' Show missing ids
  Line = 2
  For rid = 1 To max_id + 1
    If Idu(rid) = 0 Then
      Sheets("Missing").Cells(Line, 1).Value = rid
      Line = Line + 1
    End If
  Next rid
  ' Write log
  'Open Path + "\eRule.log" For Append As #1
  'Print #1, "Compile"
  'Close #1
  Call OptimizeCode_End
End Sub

Sub CopyComments(ws1, ws2)
  Worksheets(ws1).UsedRange.Copy
  Worksheets(ws2).UsedRange.PasteSpecial xlPasteComments
  Application.CutCopyMode = False
End Sub

Sub PasteSpecialFormats()
Attribute PasteSpecialFormats.VB_ProcData.VB_Invoke_Func = "d\n14"
    If Application.CutCopyMode = False Then
        Beep
        MsgBox "No formatting in Clipboard"
    Else
        Selection.PasteSpecial Paste:=xlFormats
    End If
End Sub
Sub CopyAllComments()
  scount = Worksheets.count
  src_sheet = 0
  For s = 1 To scount
    Set ws = Worksheets(s)
    ' Cycle through rules
    If IsNumeric(ws.Name) Then
      If src_sheet Then
        Call CopyComments(src_sheet, s)
      Else
        src_sheet = s
      End If
    End If
  Next s
End Sub

Sub FitComments()
'Updateby20140325
Dim xComment As Comment
  scount = Worksheets.count
  src_sheet = 0
  For s = 1 To scount
    Set ws = Worksheets(s)
    For Each xComment In ws.Comments
      xComment.Shape.TextFrame.AutoSize = True
    Next
  Next s
End Sub

Function TestFiltered() As Long
    Dim rngFilter As Range
    Dim r As Long, f As Long
    Set rngFilter = Sheets("Data").AutoFilter.Range
    r = rngFilter.Rows.count
    f = rngFilter.Columns(1).SpecialCells(xlCellTypeVisible).count
    TestFiltered = 0
    If r > f Then
      MsgBox "Cannot run - disable filter in Data sheet first"
      TestFiltered = 1
    End If
End Function

' Excel macro to export all VBA source code in this project to text files for proper source control versioning
' Requires enabling the Excel setting in Options/Trust Center/Trust Center Settings/Macro Settings/Trust access to the VBA project object model
Public Sub ExportVbaCode()
    Const Module = 1
    Const ClassModule = 2
    Const Form = 3
    Const Document = 100
    Const Padding = 24
    
    Dim VBComponent As Object
    Dim count As Integer
    Dim path As String
    Dim directory As String
    Dim extension As String
    Set fso = CreateObject("Scripting.FileSystemObject")
    
    directory = ThisWorkbook.path & "\vba-" & ThisWorkbook.Name
    If fso.FolderExists(directory) Then
      fso.deletefile directory & "\*.*", True
    End If
    count = 0
    
    If Not fso.FolderExists(directory) Then
        Call fso.CreateFolder(directory)
    End If
    Set fso = Nothing
    
    For Each VBComponent In ActiveWorkbook.VBProject.VBComponents
        Select Case VBComponent.Type
            Case ClassModule, Document
                extension = ".cls"
            Case Form
                extension = ".frm"
            Case Module
                extension = ".bas"
            Case Else
                extension = ".txt"
        End Select
            
                
        On Error Resume Next
        Err.Clear
        
        path = directory & "\" & VBComponent.Name & extension
        Call VBComponent.Export(path)
        
        If Err.Number <> 0 Then
            Call MsgBox("Failed to export " & VBComponent.Name & " to " & path, vbCritical)
        Else
            count = count + 1
            Debug.Print "Exported " & Left$(VBComponent.Name & ":" & Space(Padding), Padding) & path
        End If

        On Error GoTo 0
    Next
    
    Application.StatusBar = "Successfully exported " & CStr(count) & " VBA files to " & directory
    Application.OnTime Now + TimeSerial(0, 0, 10), "ClearStatusBar"
End Sub

Sub ClearStatusBar()
  Application.StatusBar = False
End Sub
