#!/usr/bin/python
# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'searchform.ui'
#
# Created: Tue Dec 4 19:50:45 2007
#      by: The PyQt User Interface Compiler (pyuic) 3.17.3
#
# WARNING! All changes made in this file will be lost!


import os   # os.path.exists, os.mkdir, op.popen
import sys
import md5
import time # time.clock()
from qt import *
from qttable import QTable, QTableItem, QCheckTableItem


def printf(o, s):
    if o.m__bTrace:
        print s
    o.teLog.append(s)

def getMySQLConn(Host = '####', User = '####', Password = '####', Database = '####'):
    import MySQLdb

    MySQLConn = None
    try:
        MySQLConn = MySQLdb.connect(
            host = Host,
            user = User,
            passwd = Password,
            db = Database
        )
    except MySQLdb.Error, e:
        sys.stderr.write('MySQLdb: Error Code = %d(|%s|)\n' % (e.args[0], e.args[1]))

    return MySQLConn

class myQTableItem(QTableItem):
    def __init__(self, table, text, editType = QTableItem.Never):
#       self.textType = type(text)
        QTableItem.__init__(self, table, editType, QString(str(text)))

#   def alignment(self):
#       if self.textType == type('str'):
#           return Qt.AlignLeft | Qt.AlignVCenter
#       return Qt.AlignCenter | Qt.AlignVCenter


class PopulateTableThread(QThread):
    def __init__(self, mainThread, query):
        QThread.__init__(self)

        self.mainThread     = mainThread
        self.query      = query

    def run(self):
################################################################################
        startTime = time.clock()
        mainThread = self.mainThread
        import MySQLdb
        conn = mainThread.m__MySQLConn

        searchDone      = mainThread.SEARCH_EXITED_WITH__UNDEFINED_ERROR
        cursor          = None
        try:
            try:
                table = mainThread.tblResult

                cursor = conn.cursor(MySQLdb.cursors.DictCursor)
                cursor.execute(self.query)
                NRecord = cursor.rowcount
                mainThread.teLog.append('# of rows: %d' % NRecord)
                mainThread.m__record = list()
                mainThread.m__NRecord = NRecord
                indexRow = 0
                while (1):
                    mainThread.m__mutex.lock()
                    abortValue = mainThread.m__abortValue
                    mainThread.m__mutex.unlock()
                    if abortValue != mainThread.ABORT__CONTINUE:
                        printf(mainThread, 'Populating Table thread: m__abort')
                        mainThread.pbSearch.setEnabled(False)
                        if abortValue == mainThread.ABORT__STOP:
                            printf(mainThread, 'Populating Table thread: ABORT__STOP')
                            searchDone = mainThread.SEARCH_ABORTED_BY__ABORT_STOP
                        else:   # ABORT__TERMINATE
                            printf(mainThread, 'Populating Table thread: ABORT__TERMINATE')
                            searchDone = mainThread.SEARCH_ABORTED_BY__ABORT_TERMINATE
                        return          # A loop is exited abnormally.
                                    # A cleanup is done at "finally:"

                    record = cursor.fetchone()
                    if record == None:      # A loop is exited normally.
                        mainThread.pbSearch.setEnabled(False)
                        mainThread.pbSearch.setText('Wait...')
                        searchDone = mainThread.SEARCH_DONE__OK
                        break;

                    mainThread.m__record.append(dict(record))

                    indexRow += 1

                cursor.close()
                cursor = None

            except MySQLdb.Error, e:
                printf(mainThread, 'MySQLdb: Error <font color="red">%d</font>: <font color="blue">%s</font>' % (e.args[0], e.args[1]))
                searchDone = mainThread.SEARCH_EXITED_BY__MYSQL_ERROR
            except:
                searchDone = mainThread.SEARCH_EXITED_WITH__UNDEFINED_ERROR

################################################################################

        finally:
            if cursor:
                # A loop is exited abnormally.
                try:
                    cursor.close()
                except MySQLdb.Error, e:
                    printf(mainThread, 'MySQLdb: Error <font color="red">%d</font>: <font color="blue">%s</font>' % (e.args[0], e.args[1]))
                    searchDone = mainThread.SEARCH_EXITED_BY__MYSQL_ERROR
                cursor = None

            printf(mainThread, 'Elapsed time in Populating Table thread = %.3f s' % (time.clock() - startTime))
            # Notify an end of thread.
            printf(mainThread, 'Populating Table thread: End of thread')
            event = QCustomEvent(mainThread.EVENTTYPE__POPTABLE_END)
            event.setData(searchDone)
            QApplication.postEvent(mainThread, event)


class ExportThread(QThread):
    def __init__(self, mainThread, query):
        QThread.__init__(self)

        self.mainThread     = mainThread
        self.query      = query

    def run(self):
        startTime = time.clock()
        mainThread = self.mainThread

        import MySQLdb
        conn = mainThread.m__MySQLConn

        exportDone  = mainThread.EXPORT_EXITED_WITH__UNDEFINED_ERROR
        cursor      = None
        try:
            try:
                cursor = conn.cursor(MySQLdb.cursors.SSDictCursor)
                cursor.execute(self.query)
                mainThread.prbExport.setTotalSteps(mainThread.m__NRecord)
                indexRow = 0
                yesAllToSubPathOverwrite = False
                yesAllToFullPathnameOverwrite = False
                while (1):
                    mainThread.m__mutex.lock()
                    abortValue = mainThread.m__abortValue
                    mainThread.m__mutex.unlock()
                    if abortValue != mainThread.ABORT__CONTINUE:
                        printf(mainThread, 'Export thread: m__abort')
                        mainThread.pbExport.setEnabled(False)
                        if abortValue == mainThread.ABORT__STOP:
                            printf(mainThread, 'Export thread: ABORT__STOP')
                            exportDone = mainThread.EXPORT_EXITED_BY__ABORT_STOP
                        else:   # ABORT__TERMINATE
                            printf(mainThread, 'Export thread: ABORT__TERMINATE')
                            exportDone = mainThread.EXPORT_EXITED_BY__ABORT_TERMINATE
                        return          # A loop is exited abnormally.
                                    # A cleanup is done at "finally:"

                    record = cursor.fetchone()
                    if record == None:      # A loop is exited normally.
                        mainThread.pbExport.setEnabled(False)
                        mainThread.pbExport.setText('Wait...')
                        exportDone = mainThread.EXPORT_DONE__OK
                        break;


                    # Make subpath for each MOL_ID.
                    MOL_ID = record['MOL_ID']
                    subPath = ''        # With MOL_ID...
                    iSubPath = mainThread.m__iSubPath
                    if iSubPath == mainThread.SUBPATH__MOL0PAD: # "MOL%08d" % MOL_ID
                        subPath = 'MOL%08d/' % MOL_ID
                    elif iSubPath == mainThread.SUBPATH__MOL:   # "MOL%d" % MOL_ID
                        subPath = 'MOL%d/' % MOL_ID
                    elif iSubPath == mainThread.SUBPATH__MOLONLY:   # "%d" % MOL_ID
                        subPath = '%d/' % MOL_ID
                    if subPath != '':
                        fullPath = mainThread.m__rootPath + subPath
                        if os.path.exists(fullPath):
                            if yesAllToSubPathOverwrite == False:
                                event = QCustomEvent(mainThread.EVENTTYPE__MESSAGEBOX)
                                event.setData({'Caption': 'Overwrite Directory', 'Message': '".mol2" file subpath:= |%s|: Exists: Overwrite?' % fullPath})
                                QApplication.postEvent(mainThread, event)
                                mainThread.m__warning.wait()
                                if mainThread.m__warningReturn == mainThread.WARNINGRETURN__YESTOALL:
                                    yesAllToSubPathOverwrite = True
                                elif mainThread.m__warningReturn != mainThread.WARNINGRETURN__YES:
                                    # Escape or Abort
                                    exportDone = mainThread.EXPORT_ABORTED_BY__PATH_WARNING
                                    break
                        else:
                            os.mkdir(fullPath, 0755)

                    # Base Filanme
                    ZINC_ID = record['ZINC_ID']
                    strRepr = record['Repr']
                    reprID = None   # "ref"
                    if strRepr == 'ref':
                        reprID = 0
                    elif strRepr == 'mid':
                        reprID = 1
                    elif strRepr == 'hi':
                        reprID = 2
                    elif strRepr == 'lo':
                        reprID = 3
                    Serial = record['Serial']
                    baseFilename = None # With (ZINC_ID, Repr, Serial)
                    iBaseFilename = mainThread.m__iBaseFilename
                    if iBaseFilename == mainThread.BASEFILE__ZINC0PAD_STRREPR_SER0PAD:
                        baseFilename = 'ZINC%08d-%s.%02d' % (ZINC_ID, strRepr, Serial)
                    elif iBaseFilename == mainThread.BASEFILE__ZINC0PAD_REPR0ID_SER0PAD:
                        baseFilename = 'ZINC%08d-%d.%02d' % (ZINC_ID, reprID, serial)
                    elif iBaseFilename == mainThread.BASEFILE__ZINC_STRREPR_SER:
                        baseFilename = 'ZINC%d-%s.%d' % (ZINC_ID, strRepr, serial)
                    elif iBaseFilename == mainThread.BASEFILE__ZINC_REPRID_SER:
                        baseFilename = 'ZINC%d-%d.%d' % (ZINC_ID, reprID, serial)
                    elif iBaseFilename == mainThread.BASEFILE__MOL0PAD_ZINC0PAD_STRREPR_SER0PAD:
                        baseFilename = 'MOL%08d-ZINC%08d-%s.%02d' % (MOL_ID, ZINC_ID, strRepr, serial)
                    elif iBaseFilename == mainThread.BASEFILE__MOL_ZINC0PAD_STRREPR_SER0PAD:
                        baseFilename = 'MOL%d-ZINC%08d-%s.%02d' % (MOL_ID, ZINC_ID, strRepr, serial)
                    fullPathname = mainThread.m__rootPath + subPath + baseFilename + '.mol2'
                    if os.path.exists(fullPathname) and yesAllToFullPathnameOverwrite != True:
                        event = QCustomEvent(mainThread.EVENTTYPE__MESSAGEBOX)
                        event.setData({'Caption': 'Overwrite File', 'Message': '".mol2" file:= |%s|: Exists: Overwrite?' % fullPathname})
                        QApplication.postEvent(mainThread, event)
                        mainThread.m__warning.wait()
                        if mainThread.m__warningReturn == mainThread.WARNINGRETURN__YESTOALL:
                            yesAllToFullPathnameOverwrite = True
                        elif mainThread.m__warningReturn != mainThread.WARNINGRETURN__YES:
                            # Escape or Abort
                            exportDone = mainThread.EXPORT_ABORTED_BY__FILE_WARNING
                            break
                    # Write to file.
                    f = open(fullPathname, 'w')
                    for _ in record['Content']:
                        f.write(_)
                    f.close()

                    # Check the integrity.
                    m = md5.new()
                    f = open(fullPathname)
                    for _ in f:
                        m.update(_)
                    f.close()
                    if record['MD5SUM'] != m.hexdigest():
                        printf(mainThread, 'Error: Check integrity for |%s|: Failed' % fullPathname)
                        exportDone = mainThread.EXPORT_ABORTED_BY__INTEGRITY_FAILURE
                        break


                    indexRow += 1
                    #mainThread.prbExport.setProgress(indexRow)
                    # Any operations that generate events must not be called by any thread other than the GUI thread.
                    event = QCustomEvent(mainThread.EVENTTYPE__EXPORT_RUN)
                    event.setData(indexRow)
                    QApplication.postEvent(mainThread, event)


                cursor.close()
                cursor = None

            except MySQLdb.Error, e:
                printf(mainThread, 'MySQLdb: Error <font color="red">%d</font>: <font color="blue">%s</font>' % (e.args[0], e.args[1]))
                exportDone = mainThread.EXPORT_EXITED_BY__MYSQL_ERROR
            except:
                exportDone = mainThread.EXPORT_EXITED_WITH__UNDEFINED_ERROR

        finally:
            if cursor:
                # A loop is exited abnormally;
                #   exportDone = mainThread.EXPORT_EXITED_BY__ABORT_STOP or
                #   exportDone = mainThread.EXPORT_EXITED_BY__ABORT_TERMINATE
                try:
                    cursor.close()
                except MySQLdb.Error, e:
                    printf(mainThread, 'MySQLdb: Error <font color="red">%d</font>: <font color="blue">%s</font>' % (e.args[0], e.args[1]))
                    exportDone = mainThread.EXPORT_EXITED_BY__MYSQL_ERROR
                cursor = None

            printf(mainThread, 'Elapsed time in Export thread = %.3f s' % (time.clock() - startTime))
            # Notify an end of thread.
            printf(mainThread, 'Export thread: End of thread')
            event = QCustomEvent(mainThread.EVENTTYPE__EXPORT_END)
            event.setData(exportDone)
            QApplication.postEvent(mainThread, event)


class searchForm(QDialog):
    def __del__(self):
        if self.m__bTrace:
            print 'searchForm.__del__(): ...'

    def cleanup(self):
        if self.m__isPopulateTableThreadRunning:
            printf(self, 'Populating Table thread is running')
            self.mf__abort(self.pbSearch, self.ABORT__TERMINATE)
            self.m__populateTableThread.wait()
            printf(self, 'Populating Table thread terminated')
            self.m__isPopulateTableThreadRunning = False
        if self.m__isExportThreadRunning:
            printf(self, 'Export thread is running')
            self.mf__abort(self.pbExport, self.ABORT__TERMINATE)
            self.m__exportThread.wait()
            printf(self, 'Export thread terminated')
            self.m__isExportThreadRunning = False

        return 
    def closeEvent(self, e):
#       print 'closeEvent(): ...'
        self.cleanup()
        QDialog.closeEvent(self, e)
    def keyPressEvent(self, keyEvent):
#       print 'keyPressEvent(): ...%s(%s), %s, %s' % (keyEvent.key(), Qt.Key_Escape, keyEvent.ascii(), keyEvent.text())
        if keyEvent.key() == Qt.Key_Escape:
            self.cleanup()
        QDialog.keyPressEvent(self, keyEvent)
    def keyReleaseEvent(self, e):
#       print 'keyReleaseEvent(): ...'
        QDialog.keyReleaseEvent(self, e)

    def mf__abort(self, pbXxx, abortValue):
        self.m__mutex.lock()
        pbXxx.setEnabled(False)
        pbXxx.setText('Wait...')
        self.m__abortValue = abortValue
        self.m__mutex.unlock()
    def customEvent(self, event):
        eventType = event.type()
        eventData = event.data()

        try:
#           print 'Type:= |%s|, Data:= |%s|(type:= |%s|)' % (eventType, eventData, type(eventData))
            if eventType == self.EVENTTYPE__POPTABLE_END:
                startTime = time.clock()
                searchDone = eventData
                if searchDone == self.SEARCH_DONE__OK:
                    _ = 'Populating Table thread is terminated successfully.'
                elif searchDone == self.SEARCH_ABORTED_BY__ABORT_STOP:
                    _ = 'Populating Table thread is aborted by ABORT__STOP.'
                elif searchDone == self.SEARCH_ABORTED_BY__ABORT_TERMINATE:
                    _ = 'Populating Table thread is aborted by ABORT__TERMINATE.'
                elif searchDone == self.SEARCH_EXITED_BY__MYSQL_ERROR:
                    _ = 'Populating Table thread is exited by <font color="red">MySQL Error</font>.'
                else:   # SEARCH_EXITED_WITH__UNDEFINED_ERROR
                    _ = 'Populating Table thread is exited with <font color="red">Undefined Error</font>.'
                printf(self, '%s' % _)

                self.pbSearch.setText(self.__tr('&Search'))

                if searchDone == self.SEARCH_DONE__OK:
                    _startTime = time.clock()
                    self.pbSearch.setEnabled(False)

                    table = self.tblResult
                    NRecord = self.m__NRecord
                    table.setNumRows(NRecord)
                    if NRecord:
                        self.prbSearch.setTotalSteps(NRecord)
                    byStep = 2
                    NUnit = int(NRecord / (100 / byStep))
                    for indexRow in range(NRecord):
                        record = self.m__record[indexRow]

                        # Without QTable.adjustColumn
                        #   setItem(MyQTableItem)       57.720 sec
                        #   setItem(QTableItem)     53.510 sec
                        #   setItem(QCheckTableItem)    51.090 sec
                        #   setText             40.880 sec
                        table.setText(indexRow, self.COL__MOL_ID, str(record["MOL_ID"]))
                        table.setText(indexRow, self.COL__SUBSET, record["Subset"])
                        table.setText(indexRow, self.COL__ZINC_ID, str(record["ZINC_ID"]))
                        table.setText(indexRow, self.COL__REPR, record["Repr"])
                        table.setText(indexRow, self.COL__SERIAL, str(record["Serial"]))
                        table.setText(indexRow, self.COL__MWT, str(record["MWT"]))
                        table.setText(indexRow, self.COL__XLOGP, str(record["xLogP"]))
                        table.setText(indexRow, self.COL__AP_DESOLV, str(record["ap_desolvation"]))
                        table.setText(indexRow, self.COL__P_DESOLV, str(record["p_desolvation"]))
                        table.setText(indexRow, self.COL__HBD, str(record["HBD"]))
                        table.setText(indexRow, self.COL__HBA, str(record["HBA"]))
                        table.setText(indexRow, self.COL__TPSA, str(record["tPSA"]))
                        table.setText(indexRow, self.COL__CHARGE, str(record["Charge"]))
                        table.setText(indexRow, self.COL__NRB, str(record["NRB"]))
                        table.setText(indexRow, self.COL__SMILES, record["SMILES"])
                        table.setText(indexRow, self.COL__NAME, record["Name"])

                        indexRow += 1
                        if NUnit:
                            if indexRow == NUnit * int(indexRow / NUnit):
#                               print 'NUnit = %d, indexRow = %d (%d %%)' % (NUnit, indexRow, indexRow / NUnit * byStep)
                                self.prbSearch.setProgress(indexRow)
                                pass
                        else:
                            self.prbSearch.setProgress(indexRow)
                            pass
                    if NRecord:
                        self.prbSearch.setProgress(NRecord)

                    for iCol in range(table.numCols()):
                        table.adjustColumn(iCol)

                    if NRecord:
                        self.ckbExport.setText(self.__tr("E&xport..."))
                        self.ckbExport.setEnabled(True)
                    else:
                        self.ckbExport.setText(self.__tr("Nothing to export"))
                        self.ckbExport.setEnabled(False)

                    self.pbSearch.setEnabled(True)
                    printf(self, 'Elapsed time in populating table = %.3f s' % (time.clock() - _startTime))
                else:
                    self.m__NRecord = None
                self.m__record = list()     # Release memory.

                self.m__bExportOpen = False
                self.m__bSearchDone = searchDone == self.SEARCH_DONE__OK
                self.setWidgetStatesInResultTab()
                if searchDone == self.SEARCH_DONE__OK:
                    # Show "Result" tab.
                    self.twSearch.setCurrentPage(1)
                else:
                    # Reset QProgressBar.
                    self.prbSearch.reset()
                    if \
                        searchDone == self.SEARCH_EXITED_BY__MYSQL_ERROR or \
                        searchDone == self.SEARCH_EXITED_WITH__UNDEFINED_ERROR:
                        # Show "Log" tab.
                        self.twSearch.setCurrentPage(2)

                self.m__isPopulateTableThreadRunning = False
                self.pbClearAll.setEnabled(True)
                self.pbSearch.setEnabled(True)
                printf(self, 'Elapsed time in EVENTTYPE__POPTABLE_END = %.3f s' % (time.clock() - startTime))
            elif eventType == self.EVENTTYPE__EXPORT_END:
                if self.m__NRecord:
                    self.prbExport.setProgress(self.m__NRecord)

                exportDone = eventData
                if exportDone == self.EXPORT_DONE__OK:
                    _ = 'Export thread is terminated successfully.'
                elif exportDone == self.EXPORT_EXITED_BY__ABORT_STOP:
                    _ = 'Export thread is exited by ABORT__STOP.'
                elif exportDone == self.EXPORT_EXITED_BY__ABORT_TERMINATE:
                    _ = 'Export thread is exited by ABORT__TERMINATE.'
                elif exportDone == self.EXPORT_ABORTED_BY__PATH_WARNING:
                    _ = 'Export thread is aborted by PATH_WARNING.'
                elif exportDone == self.EXPORT_ABORTED_BY__FILE_WARNING:
                    _ = 'Export thread is aborted by FILE_WARNING.'
                elif exportDone == self.EXPORT_ABORTED_BY__INTEGRITY_FAILURE:
                    _ = 'Export thread is aborted by INTEGRITY_FAILURE.'
                elif exportDone == self.EXPORT_EXITED_BY__MYSQL_ERROR:
                    _ = 'Export thread is exited by <font color="red">MySQL Error</font>.'
                else:   # EXPORT_EXITED_WITH__UNDEFINED_ERROR
                    _ = 'Export thread is exited with <font color="red">Undefined Error</font>.'
                printf(self, '%s' % _)

                self.pbExport.setText(self.__tr('&Export'))

                if exportDone != self.EXPORT_DONE__OK:
                    # Reset QProgressBar.
                    self.prbExport.reset()
                    if \
                        exportDone == self.EXPORT_ABORTED_BY__INTEGRITY_FAILURE or \
                        exportDone == self.EXPORT_EXITED_BY__MYSQL_ERROR or \
                        exportDone == self.EXPORT_EXITED_WITH__UNDEFINED_ERROR:
                        # Show "Log" tab.
                        self.twSearch.setCurrentPage(2)

                self.m__isExportThreadRunning = False
                self.pbSearch.setEnabled(True)
                self.pbClearAll.setEnabled(True)
                self.pbExport.setEnabled(True)
            elif eventType == self.EVENTTYPE__EXPORT_RUN:
                byStep = 2
                NUnit = int(self.m__NRecord / (100 / byStep))
                indexRow = eventData
                if NUnit:
                    if indexRow == NUnit * int(indexRow / NUnit):
#                       print 'NUnit = %d, indexRow = %d (%d %%)' % (NUnit, indexRow, indexRow / NUnit * byStep)
                        self.prbExport.setProgress(indexRow)
                else:
                    self.prbExport.setProgress(indexRow)
            elif eventType == self.EVENTTYPE__MESSAGEBOX:
                self.m__warningReturn = QMessageBox.warning(
                    None,
                    self.__tr(eventData['Caption']),
                    self.__tr(eventData['Message']),
                    'Abor&t',       # WARNINGRETURN__ABORT
                    '&Yes',         # WARNINGRETURN__YES
                    'Yes to &All',      # WARNINGRETURN__YESTOALL
                    self.WARNINGRETURN__DEFAULT,
                    self.WARNINGRETURN__ESCAPE,
                )
                if self.m__warningReturn == self.WARNINGRETURN__ABORT:
                    self.pbExport.setText('Wait...')
                    self.pbExport.setEnabled(False)
                self.m__warning.wakeOne()
            else:
                printf(self, 'Warning: Unknown Event Type:= |%s|, Data:= |%s|(Type:= |%s|)' % (eventType, eventData, type(eventData)))

        finally:
            del eventData

    def __init__(self, Argv, parent = None,name = None,modal = 0,fl = 0):
        QDialog.__init__(self,parent,name,modal,fl)

        self.m__bTrace      = False
        for iArg in range(1, len(Argv)):
#           print 'Argv[%d]:= |%s|' % (iArg, Argv[iArg])
            Args = Argv[iArg]
            if Args == '--trace':
                self.m__bTrace = True
            else:
                printf(self, 'Warning: Argv[%d]:= |%s|: Unknown argument: Ignored' % (iArgv, Args))
########################################################################
        # MySQL...
        self.m__MySQLConn = getMySQLConn()
        if not self.m__MySQLConn:
            sys.stderr.write('Error: MySQL: Failed to connect to MySQL Server\n')
            sys.exit(1)


        # Threads...
        # From QCustomEvent Class Reference:
        #   To avoid clashes with the Qt-defined events types,
        #   the value should be at least as large as the value of the "User" entry in the QEvent::Type enum.
        # From "Thread Support in Qt":
        #   Any operations that generate events must not be called by any thread other than the GUI thread.
        # From "http://lists.trolltech.com/qt-interest/2001-01/thread00930-0.html"
        #   ...which means that the messagebox is created from a secondary thread. ...
        #   ... it is not safe (read: not possible) to open a widget in a secondary thread. ...
        self.EVENTTYPE__POPTABLE_END    = QEvent.User + 0
        self.SEARCH_EXITED_WITH__UNDEFINED_ERROR    = -1    # Undefined error
        self.SEARCH_DONE__OK        = 0
        self.SEARCH_ABORTED_BY__ABORT_STOP      = 1
        self.SEARCH_ABORTED_BY__ABORT_TERMINATE     = 2
        self.SEARCH_EXITED_BY__MYSQL_ERROR      = 3
        self.EVENTTYPE__EXPORT_END  = QEvent.User + 1
        self.EXPORT_EXITED_WITH__UNDEFINED_ERROR    = -1    # Undefined error
        self.EXPORT_DONE__OK                = 0 # A loop is exited normally.
                                    # A loop is exited abnormally:
        self.EXPORT_EXITED_BY__ABORT_STOP       = 1 #   by ABORT__STOP
        self.EXPORT_EXITED_BY__ABORT_TERMINATE      = 2 #   by ABORT__TERMINATE
        self.EXPORT_ABORTED_BY__PATH_WARNING        = 3 #   by Abort in "Overwrite Directory"
        self.EXPORT_ABORTED_BY__FILE_WARNING        = 4 #   by Abort in "Overwrite File"
        self.EXPORT_ABORTED_BY__INTEGRITY_FAILURE   = 5 #   by mismatch in the integrity of ".mol2" file
        self.EXPORT_EXITED_BY__MYSQL_ERROR      = 6 #   by MySQL error
        self.EVENTTYPE__EXPORT_RUN  = QEvent.User + 2
        self.EVENTTYPE__MESSAGEBOX  = QEvent.User + 3
        self.m__mutex = QMutex()
        self.m__abort           = QWaitCondition()
        self.ABORT__CONTINUE        = 0
        self.ABORT__STOP        = 1
        self.ABORT__TERMINATE       = 2
        self.m__abortValue      = None  # ABORT__XXX
        self.m__warning         = QWaitCondition()
        self.WARNINGRETURN__ABORT   = 0
        self.WARNINGRETURN__YES     = 1
        self.WARNINGRETURN__YESTOALL    = 2
        self.WARNINGRETURN__DEFAULT = self.WARNINGRETURN__YESTOALL
        self.WARNINGRETURN__ESCAPE  = self.WARNINGRETURN__ABORT
        self.m__warningReturn       = None  # WARNINGRETURN__XXX
        self.m__populateTableThread     = None
        self.m__isPopulateTableThreadRunning    = False
        self.m__exportThread            = None
        self.m__isExportThreadRunning       = False

        # Define variables and set default values...
        # "Search" tab...
        # "ID"...
        self.m__byID = False
        # Two variables below are mutually exclusive and valid only when m__byID = True
        self.m__byMOL_ID = False
        self.m__byZINC_ID = True

        # "Subset"...
        # Valid only when m__byID = False
        # Define "SUBSET__XXX" constants.
        self.SUBSET__ANY    = 0 # Any subset
        self.SUBSET__NCI1990    = 1 # "NCI 1990 subset" only
        self.SUBSET__CF     = 2 # "clean-fragments subset" only
        self.SUBSET__BOTH   = 3 # Both subset
        self.m__subset = []
        self.m__subset.insert(self.SUBSET__ANY, 'Any')
        self.m__subset.insert(self.SUBSET__NCI1990, 'NCI 1990')
        self.m__subset.insert(self.SUBSET__CF, 'clean-fragments')
#       self.m__subset.insert(self.SUBSET__BOTH, 'Both')
        self.m__iSubset = self.SUBSET__NCI1990

        # "Representative"...
        # Valid when m__byID = False or m__byZINC_ID = True
        # Define "REPR__XXX" constants.
        self.REPR__REF      = 0 # "Reference(ref)"
        self.REPR__MID      = 1 # "medium(mid)"
        self.REPR__HI       = 2 # "high(hi)"
        self.REPR__LO       = 3 # "low(lo)"
        self.REPR__SINGLE   = 4 # "Single(ref)"
        self.REPR__USUAL    = 5 # "Usual(Single + medium)"
        self.REPR__METAL    = 6 # "Metals(Usual + high)"
        self.REPR__ALL      = 7 # "All(Metals + low)"
        self.m__repr = []
        self.m__repr.insert(self.REPR__REF, 'Reference(ref)')
        self.m__repr.insert(self.REPR__MID, 'medium(mid)')
        self.m__repr.insert(self.REPR__HI, 'high(hi)')
        self.m__repr.insert(self.REPR__LO, 'low(lo)')
        self.m__repr.insert(self.REPR__SINGLE, 'Single(ref)')
        self.m__repr.insert(self.REPR__USUAL, 'Usual(Single + medium)')
        self.m__repr.insert(self.REPR__METAL, 'Metals(Usual + high)')
        self.m__repr.insert(self.REPR__ALL, 'All(Metals + low)')
        self.m__iRepr = self.REPR__ALL

        # Variable below is valid only when m__byID = True;
        # set default to '', which means any ID.
        self.m__ID = '119492'       # string: MOL_ID or ZINC_ID

        # Six variables below are valid only when m__byID = False;
        # set default to '', which means any value.
        self.m__minxLogP = ''       # float: minimum xLogP
        self.m__maxxLogP = ''       # float: maximum xLogP
        self.m__minCharge = ''      # float: minimum Charge
        self.m__maxCharge = ''      # float: maximum Charge
        self.m__minNRB = ''     # float: mininum NRB (# of Torsion)
        self.m__maxNRB = ''     # float: maximum NRB

        # "Result" tab...
        self.m__bSearchDone = False
        self.m__record = list()     # Array of record
        self.m__NRecord = 0     # Number of data:= len(self.m__record)
        self.m__bExportOpen = False # Test if |Export ".mol2"| group box is opened or not.
        self.m__rootPath = os.path.abspath(os.path.curdir) + '/MOL2.d/'
        # "Sub Pathname"...
        # Define "SUBPATH__XXX" constants.
        self.SUBPATH__MOL0PAD   = 0 # "MOL%08d" % MOL_ID
        self.SUBPATH__MOL   = 1 # "MOL%d" % MOL_ID
        self.SUBPATH__MOLONLY   = 2 # "%d" % MOL_ID
        self.SUBPATH__NONE  = 3 # Without sub-pathname
        self.m__subPath = []
        self.m__subPath.insert(self.SUBPATH__MOL0PAD, 'MOL#')
        self.m__subPath.insert(self.SUBPATH__MOL, 'MOL%')
        self.m__subPath.insert(self.SUBPATH__MOLONLY, '%')
        self.m__subPath.insert(self.SUBPATH__NONE, 'None')
        self.m__iSubPath = self.SUBPATH__NONE
        # "Base Filename"...
        # Define "BASEFILE__XXX" constants.
        self.BASEFILE__ZINC0PAD_STRREPR_SER0PAD     = 0 # "ZINC%08d-%s.%02d" % (ZINC_ID, strRepr, serial)
        self.BASEFILE__ZINC0PAD_REPR0ID_SER0PAD     = 1 # "ZINC%08d-%d.%02d" % (ZINC_ID, reprID, serial)
        self.BASEFILE__ZINC_STRREPR_SER         = 2 # "ZINC%d-%s.%d" % (ZINC_ID, strRepr, serial)
        self.BASEFILE__ZINC_REPRID_SER          = 3 # "ZINC%d-%d.%d" % (ZINC_ID, reprID, serial)
        self.BASEFILE__MOL0PAD_ZINC0PAD_STRREPR_SER0PAD = 4 # "MOL%08d-ZINC%08d-%s.%02d" % (MOL_ID, ZINC_ID, strRepr, serial)
        self.BASEFILE__MOL_ZINC0PAD_STRREPR_SER0PAD = 5 # "MOL%d-ZINC%08d-%s.%02d" % (MOL_ID, ZINC_ID, strRepr, serial)
        # More...
        self.m__baseFilename = []
        self.m__baseFilename.insert(self.BASEFILE__ZINC0PAD_STRREPR_SER0PAD, 'ZINC#-repr.#')
        self.m__baseFilename.insert(self.BASEFILE__ZINC0PAD_REPR0ID_SER0PAD, 'ZINC#-%.#')
        self.m__baseFilename.insert(self.BASEFILE__ZINC_STRREPR_SER, 'ZINC%-repr.%')
        self.m__baseFilename.insert(self.BASEFILE__ZINC_REPRID_SER, 'ZINC%-%.%')
        self.m__baseFilename.insert(self.BASEFILE__MOL0PAD_ZINC0PAD_STRREPR_SER0PAD, 'MOL#-ZINC#-repr.#')
        self.m__baseFilename.insert(self.BASEFILE__MOL_ZINC0PAD_STRREPR_SER0PAD, 'MOL%-ZINC#-repr.#')
        self.m__iBaseFilename = self.BASEFILE__ZINC0PAD_STRREPR_SER0PAD

        # DB variables...
        self.m__whereCondition = ''

        # Define "COL__XXX" constants.
        self.COL__MOL_ID    = 0
        self.COL__SUBSET    = 1
        self.COL__ZINC_ID   = 2
        self.COL__REPR      = 3
        self.COL__SERIAL    = 4
        self.COL__MWT       = 5
        self.COL__XLOGP     = 6
        self.COL__AP_DESOLV = 7
        self.COL__P_DESOLV  = 8
        self.COL__HBD       = 9
        self.COL__HBA       = 10
        self.COL__TPSA      = 11
        self.COL__CHARGE    = 12
        self.COL__NRB       = 13
        self.COL__SMILES    = 14
        self.COL__NAME      = 15
########################################################################
        if not name:
            self.setName("SearchForm")


        SearchFormLayout = QGridLayout(self,1,1,11,6,"SearchFormLayout")

        self.twSearch = QTabWidget(self,"twSearch")

        self.tab = QWidget(self.twSearch,"tab")
        tabLayout = QGridLayout(self.tab,1,1,11,6,"tabLayout")
        spacer7 = QSpacerItem(20,16,QSizePolicy.Minimum,QSizePolicy.Expanding)
        tabLayout.addItem(spacer7,3,0)

        self.pbSearch = QPushButton(self.tab,"pbSearch")
#       self.pbSearch.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.pbSearch.sizePolicy().hasHeightForWidth()))
        self.pbSearch.setMinimumSize(QSize(100,0))

        tabLayout.addWidget(self.pbSearch,4,0)

        self.prbSearch = QProgressBar(self.tab,"prbSearch")

        tabLayout.addWidget(self.prbSearch,4,1)

        self.gbProperty = QGroupBox(self.tab,"gbProperty")
        self.gbProperty.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.gbProperty.sizePolicy().hasHeightForWidth()))

        self.textLabel1 = QLabel(self.gbProperty,"textLabel1")
        self.textLabel1.setGeometry(QRect(100,20,80,20))
        self.textLabel1.setAlignment(QLabel.AlignCenter)

        self.textLabel3 = QLabel(self.gbProperty,"textLabel3")
        self.textLabel3.setGeometry(QRect(100,80,80,20))
        self.textLabel3.setAlignment(QLabel.AlignCenter)

        self.textLabel2 = QLabel(self.gbProperty,"textLabel2")
        self.textLabel2.setGeometry(QRect(100,50,80,20))
        self.textLabel2.setAlignment(QLabel.AlignCenter)

        self.leMinxLogP = QLineEdit(self.gbProperty,"leMinxLogP")
        self.leMinxLogP.setGeometry(QRect(20,20,70,24))
        self.leMinxLogP.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.leMinxLogP.sizePolicy().hasHeightForWidth()))

        self.leMinCharge = QLineEdit(self.gbProperty,"leMinCharge")
        self.leMinCharge.setGeometry(QRect(20,50,70,24))
        self.leMinCharge.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.leMinCharge.sizePolicy().hasHeightForWidth()))

        self.leMaxCharge = QLineEdit(self.gbProperty,"leMaxCharge")
        self.leMaxCharge.setGeometry(QRect(190,50,70,24))
        self.leMaxCharge.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.leMaxCharge.sizePolicy().hasHeightForWidth()))

        self.leMinNRB = QLineEdit(self.gbProperty,"leMinNRB")
        self.leMinNRB.setGeometry(QRect(20,80,70,24))
        self.leMinNRB.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.leMinNRB.sizePolicy().hasHeightForWidth()))

        self.leMaxNRB = QLineEdit(self.gbProperty,"leMaxNRB")
        self.leMaxNRB.setGeometry(QRect(190,80,70,24))
        self.leMaxNRB.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.leMaxNRB.sizePolicy().hasHeightForWidth()))

        self.leMaxxLogP = QLineEdit(self.gbProperty,"leMaxxLogP")
        self.leMaxxLogP.setGeometry(QRect(190,20,70,24))
        self.leMaxxLogP.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.leMaxxLogP.sizePolicy().hasHeightForWidth()))

        tabLayout.addMultiCellWidget(self.gbProperty,2,2,0,1)

        layout8 = QHBoxLayout(None,0,6,"layout8")

        self.gbSubset = QGroupBox(self.tab,"gbSubset")
        self.gbSubset.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.gbSubset.sizePolicy().hasHeightForWidth()))
        self.gbSubset.setColumnLayout(0,Qt.Vertical)
        self.gbSubset.layout().setSpacing(6)
        self.gbSubset.layout().setMargin(11)
        gbSubsetLayout = QGridLayout(self.gbSubset.layout())
        gbSubsetLayout.setAlignment(Qt.AlignTop)

        self.cmbSubset = QComboBox(0,self.gbSubset,"cmbSubset")

        gbSubsetLayout.addWidget(self.cmbSubset,0,0)
        layout8.addWidget(self.gbSubset)

        self.gbRepr = QGroupBox(self.tab,"gbRepr")
        self.gbRepr.setColumnLayout(0,Qt.Vertical)
        self.gbRepr.layout().setSpacing(6)
        self.gbRepr.layout().setMargin(11)
        gbReprLayout = QGridLayout(self.gbRepr.layout())
        gbReprLayout.setAlignment(Qt.AlignTop)

        self.cmbRepr = QComboBox(0,self.gbRepr,"cmbRepr")

        gbReprLayout.addWidget(self.cmbRepr,0,0)
        layout8.addWidget(self.gbRepr)
        spacer14 = QSpacerItem(75,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        layout8.addItem(spacer14)

        tabLayout.addMultiCellLayout(layout8,1,1,0,1)

        self.ckbByID = QCheckBox(self.tab,"ckbByID")

        tabLayout.addWidget(self.ckbByID,0,0)

        self.gbID = QGroupBox(self.tab,"gbID")
        self.gbID.setSizePolicy(QSizePolicy(QSizePolicy.Preferred,QSizePolicy.Fixed,0,0,self.gbID.sizePolicy().hasHeightForWidth()))
        self.gbID.setFrameShape(QGroupBox.NoFrame)
        self.gbID.setColumnLayout(0,Qt.Vertical)
        self.gbID.layout().setSpacing(6)
        self.gbID.layout().setMargin(0)
        gbIDLayout = QGridLayout(self.gbID.layout())
        gbIDLayout.setAlignment(Qt.AlignTop)

        self.bgID = QButtonGroup(self.gbID,"bgID")
        self.bgID.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.bgID.sizePolicy().hasHeightForWidth()))
        self.bgID.setColumnLayout(0,Qt.Vertical)
        self.bgID.layout().setSpacing(6)
        self.bgID.layout().setMargin(11)
        bgIDLayout = QGridLayout(self.bgID.layout())
        bgIDLayout.setAlignment(Qt.AlignTop)

        self.rbMOL_ID = QRadioButton(self.bgID,"rbMOL_ID")
        self.rbMOL_ID.setChecked(1)

        bgIDLayout.addWidget(self.rbMOL_ID,0,0)

        self.rbZINC_ID = QRadioButton(self.bgID,"rbZINC_ID")

        bgIDLayout.addWidget(self.rbZINC_ID,0,1)

        gbIDLayout.addWidget(self.bgID,0,0)

        self.leID = QLineEdit(self.gbID,"leID")
        self.leID.setReadOnly(1)

        gbIDLayout.addWidget(self.leID,0,1)

        tabLayout.addWidget(self.gbID,0,1)
        self.twSearch.insertTab(self.tab,QString.fromLatin1(""))

        self.tab_2 = QWidget(self.twSearch,"tab_2")
        tabLayout_2 = QGridLayout(self.tab_2,1,1,11,6,"tabLayout_2")

        self.gbTable = QGroupBox(self.tab_2,"gbTable")
        self.gbTable.setColumnLayout(0,Qt.Vertical)
        self.gbTable.layout().setSpacing(6)
        self.gbTable.layout().setMargin(11)
        gbTableLayout = QGridLayout(self.gbTable.layout())
        gbTableLayout.setAlignment(Qt.AlignTop)

        self.tblResult = QTable(self.gbTable,"tblResult")
        """self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("MOL ID"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("Subset"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("ZINC ID"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("Repr"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("Serial"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("MWT"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("xLogP"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("ap_Desov."))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("p_Desolv."))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("HBD"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("HBA"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("tPSA"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("Charge"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("NRB"))
        self.tblResult.setNumCols(self.tblResult.numCols() + 1)
        self.tblResult.horizontalHeader().setLabel(self.tblResult.numCols() - 1,self.__tr("SMILES"))"""
        self.tblResult.setNumRows(0)
        self.tblResult.setNumCols(16)
        self.tblResult.setReadOnly(True)

        gbTableLayout.addWidget(self.tblResult,1,0)

        layout7 = QHBoxLayout(None,0,6,"layout7")

        spacer10 = QSpacerItem(200,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        layout7.addItem(spacer10)

        self.ckbExport = QCheckBox(self.gbTable,"ckbExport")
        layout7.addWidget(self.ckbExport)

        gbTableLayout.addLayout(layout7,0,0)

        tabLayout_2.addWidget(self.gbTable,0,0)

        self.gbExport = QGroupBox(self.tab_2,"gbExport")
        self.gbExport.setColumnLayout(0,Qt.Vertical)
        self.gbExport.layout().setSpacing(6)
        self.gbExport.layout().setMargin(11)
        gbExportLayout = QGridLayout(self.gbExport.layout())
        gbExportLayout.setAlignment(Qt.AlignTop)

        self.textLabel6 = QLabel(self.gbExport,"textLabel6")

        gbExportLayout.addWidget(self.textLabel6,0,0)

        layout16 = QHBoxLayout(None,0,6,"layout16")

        layout14 = QHBoxLayout(None,0,6,"layout14")

        self.textLabel5 = QLabel(self.gbExport,"textLabel5")
        layout14.addWidget(self.textLabel5)

        self.cmbSubPath = QComboBox(0,self.gbExport,"cmbSubPath")
        layout14.addWidget(self.cmbSubPath)
        layout16.addLayout(layout14)

        layout15 = QHBoxLayout(None,0,6,"layout15")

        self.textLabel7 = QLabel(self.gbExport,"textLabel7")
        layout15.addWidget(self.textLabel7)

        self.cmbBaseFilename = QComboBox(0,self.gbExport,"cmbBaseFilename")
        layout15.addWidget(self.cmbBaseFilename)
        layout16.addLayout(layout15)
        spacer9 = QSpacerItem(21,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        layout16.addItem(spacer9)

        gbExportLayout.addMultiCellLayout(layout16,1,1,0,2)

        self.leRootPath = QLineEdit(self.gbExport,"leRootPath")
        self.leRootPath.setReadOnly(True)

        gbExportLayout.addWidget(self.leRootPath,0,1)

        self.pbBrowse = QPushButton(self.gbExport,"pbBrowse")

        gbExportLayout.addWidget(self.pbBrowse,0,2)

        self.pbExport = QPushButton(self.gbExport,"pbExport")
        self.pbExport.setMinimumSize(QSize(100,0))

        gbExportLayout.addWidget(self.pbExport,3,0)

        self.tlSubFullPathname = QLabel(self.gbExport,"tlSubFullPathname")

        gbExportLayout.addMultiCellWidget(self.tlSubFullPathname,2,2,0,1)

        self.prbExport = QProgressBar(self.gbExport,"prbExport")

        gbExportLayout.addMultiCellWidget(self.prbExport,3,3,1,2)

        tabLayout_2.addWidget(self.gbExport,1,0)
        self.twSearch.insertTab(self.tab_2,QString.fromLatin1(""))

        self.TabPage = QWidget(self.twSearch,"TabPage")
        TabPageLayout = QGridLayout(self.TabPage,1,1,11,6,"TabPageLayout")

        self.teLog = QTextEdit(self.TabPage,"teLog")
        self.teLog.setTextFormat(QTextEdit.LogText)
        self.teLog.setReadOnly(1)

        TabPageLayout.addMultiCellWidget(self.teLog,0,0,0,2)
        spacer11 = QSpacerItem(280,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        TabPageLayout.addItem(spacer11,1,2)

        self.pbClearAll = QPushButton(self.TabPage,"pbClearAll")

        TabPageLayout.addWidget(self.pbClearAll,1,0)

#       self.pbSave = QPushButton(self.TabPage,"pbSave")

#       TabPageLayout.addWidget(self.pbSave,1,1)
        self.twSearch.insertTab(self.TabPage,QString.fromLatin1(""))

        SearchFormLayout.addWidget(self.twSearch,0,0)

        self.languageChange()

        self.resize(QSize(700,445).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)

        self.connect(self.cmbSubset,SIGNAL("activated(int)"),self.fnSubset)
        self.connect(self.cmbRepr,SIGNAL("activated(int)"),self.fnRepr)
        self.connect(self.ckbByID,SIGNAL("clicked()"),self.fnByID)
        self.connect(self.rbMOL_ID,SIGNAL("clicked()"),self.fnByMOL_ID)
        self.connect(self.rbZINC_ID,SIGNAL("clicked()"),self.fnByZINC_ID)
        self.connect(self.pbSearch,SIGNAL("clicked()"),self.fnSearch)
        self.connect(self.pbBrowse,SIGNAL("clicked()"),self.fnBrowse)
        self.connect(self.cmbSubPath,SIGNAL("activated(int)"),self.fnSubPathname)
        self.connect(self.cmbBaseFilename,SIGNAL("activated(int)"),self.fnBaseFilename)
        self.connect(self.pbExport,SIGNAL("clicked()"),self.fnExport)
        self.connect(self.pbClearAll,SIGNAL("clicked()"),self.fnClearAll)
        self.connect(self.ckbExport,SIGNAL("clicked()"),self.fnExportOpen)

        self.setTabOrder(self.leID,self.leMinNRB)
        self.setTabOrder(self.leMinNRB,self.leMaxNRB)
        self.setTabOrder(self.leMaxNRB,self.ckbByID)
        self.setTabOrder(self.ckbByID,self.rbMOL_ID)
        self.setTabOrder(self.rbMOL_ID,self.leMinxLogP)
        self.setTabOrder(self.leMinxLogP,self.leMaxxLogP)
        self.setTabOrder(self.leMaxxLogP,self.leMinCharge)
        self.setTabOrder(self.leMinCharge,self.leMaxCharge)
        self.setTabOrder(self.leMaxCharge,self.pbSearch)
        self.setTabOrder(self.pbSearch,self.cmbSubset)
        self.setTabOrder(self.cmbSubset,self.cmbRepr)
        self.setTabOrder(self.cmbRepr,self.leRootPath)
        self.setTabOrder(self.leRootPath,self.pbBrowse)
        self.setTabOrder(self.pbBrowse,self.cmbSubPath)
        self.setTabOrder(self.cmbSubPath,self.cmbBaseFilename)
        self.setTabOrder(self.cmbBaseFilename,self.pbExport)
        self.setTabOrder(self.pbExport,self.tblResult)
        self.setTabOrder(self.tblResult,self.teLog)
        self.setTabOrder(self.teLog,self.pbClearAll)
        self.setTabOrder(self.pbClearAll,self.twSearch)
#       self.setTabOrder(self.pbClearAll,self.pbSave)
#       self.setTabOrder(self.pbSave,self.twSearch)
########################################################################
        # Set numeric validators...
        self.leMinxLogP.setValidator(QDoubleValidator(self.leMinxLogP));
        self.leMaxxLogP.setValidator(QDoubleValidator(self.leMaxxLogP));
        self.leMinCharge.setValidator(QDoubleValidator(self.leMinCharge));
        self.leMaxCharge.setValidator(QDoubleValidator(self.leMaxCharge));
        self.leMinNRB.setValidator(QDoubleValidator(self.leMinNRB));
        self.leMaxNRB.setValidator(QDoubleValidator(self.leMaxNRB));

        # Set widget states depending on variables...
        self.setWidgetStatesInSearchTab()
        self.setWidgetStatesInResultTab()


    def setWidgetStatesInSearchTab(self):
        if self.m__byID:
            self.ckbByID.setChecked(True)
            self.gbSubset.hide()
            self.leID.setText(self.m__ID)
            if self.m__byMOL_ID:
                self.rbMOL_ID.setChecked(True)
                self.gbRepr.hide()
            else:
                self.rbZINC_ID.setChecked(True)
                self.gbRepr.show()
            self.bgID.show()
            self.leID.show()
            self.leID.setReadOnly(False)
            self.leID.setFocus()
            self.gbProperty.hide()
        else:
            self.ckbByID.setChecked(False)
            self.gbSubset.show()
            self.gbRepr.show()
            self.bgID.hide()
####            self.leID.setReadOnly(True)     # Still set focus to leID.
            self.leID.hide()
            self.leMinNRB.setFocus()
            self.gbProperty.show()


    def setWidgetStatesInResultTab(self):
        if not self.m__bSearchDone:
            self.gbExport.hide()
            self.gbTable.hide()
            return

        self.gbTable.show()
        if self.m__bExportOpen:
            self.ckbExport.setChecked(True)
            self.gbExport.show()
            self.leRootPath.setText(self.m__rootPath)
            self.leRootPath.setFocus()
            self.cmbSubPath.setCurrentItem(self.m__iSubPath)
            self.cmbBaseFilename.setCurrentItem(self.m__iBaseFilename)

            self.setSubFullPathname()

            self.pbExport.setEnabled(self.m__rootPath.strip() != '')
            if self.pbExport.isEnabled():
                QToolTip.remove(self.pbExport)
            else:
                QToolTip.add(self.pbExport, self.__tr('To enable, set "Root Pathname"'))
        else:
            self.ckbExport.setChecked(False)
            self.gbExport.hide()

    def setSubFullPathname(self):
        subFullPathname = 'Sub-full Pathname:= |'
        index = self.cmbSubPath.currentItem()
        if index == self.SUBPATH__MOL0PAD:  # "MOL%08d" % MOL_ID
            subFullPathname += 'MOL00001219/'
        elif index == self.SUBPATH__MOL:    # "MOL%d" % MOL_ID
            subFullPathname += 'MOL1219/'
        elif index == self.SUBPATH__MOLONLY:    # "%d" % MOL_ID
            subFullPathname += '1219/'
        index = self.cmbBaseFilename.currentItem()
        if index == self.BASEFILE__ZINC0PAD_STRREPR_SER0PAD:        # "ZINC%08d-%s.%02d' % (ZINC_ID, strRepr, serial)
            subFullPathname += 'ZINC00000907-ref.01.mol2'
        elif index == self.BASEFILE__ZINC0PAD_REPR0ID_SER0PAD:      # "ZINC%08d-%d.%02d' % (ZINC_ID, reprID, serial)
            subFullPathname += 'ZINC00000907-0.01.mol2'
        elif index == self.BASEFILE__ZINC_STRREPR_SER:          # "ZINC%d-%s.%d' % (ZINC_ID, strRepr, serial)
            subFullPathname += 'ZINC907-ref.1.mol2'
        elif index == self.BASEFILE__ZINC_REPRID_SER:           # "ZINC%d-%d.%d' % (ZINC_ID, reprID, serial)
            subFullPathname += 'ZINC907-0.1.mol2'
        elif index == self.BASEFILE__MOL0PAD_ZINC0PAD_STRREPR_SER0PAD:  # "MOL%08d-ZINC%08d-%s.%02d' % (MOL_ID, ZINC_ID, strRepr, serial)
            subFullPathname += 'MOL00001219-ZINC00000907-ref.01.mol2'
        elif index == self.BASEFILE__MOL_ZINC0PAD_STRREPR_SER0PAD:  # "MOL%d-ZINC%08d-%s.%02d' % (MOL_ID, ZINC_ID, strRepr, serial)
            subFullPathname += 'MOL1219-ZINC00000907-ref.01.mol2'
        subFullPathname += '|'

        self.tlSubFullPathname.setText(subFullPathname)
########################################################################


    def languageChange(self):
        self.setCaption(self.__tr("Ligand DB Search"))
        self.pbSearch.setText(self.__tr("&Search"))
        self.gbProperty.setTitle(self.__tr("Property"))
        self.textLabel1.setText(self.__tr("xLogP"))
        self.textLabel3.setText(self.__tr("# of Torsion"))
        self.textLabel2.setText(self.__tr("Charge"))
        self.gbSubset.setTitle(self.__tr("S&ubset"))
        self.cmbSubset.clear()
        """self.cmbSubset.insertItem(self.__tr("Any"))
        self.cmbSubset.insertItem(self.__tr("NCI 1990"))
        self.cmbSubset.insertItem(self.__tr("clean-fragments"))
        self.cmbSubset.insertItem(self.__tr("Both"))"""
        for _ in self.m__subset:
            self.cmbSubset.insertItem(self.__tr(_))
        self.cmbSubset.setCurrentItem(self.m__iSubset)
        self.gbRepr.setTitle(self.__tr("&Representative"))
        self.cmbRepr.clear()
        """self.cmbRepr.insertItem(self.__tr("Reference(ref)"))
        self.cmbRepr.insertItem(self.__tr("medium(mid)"))
        self.cmbRepr.insertItem(self.__tr("high(hi)"))
        self.cmbRepr.insertItem(self.__tr("low(lo)"))
        self.cmbRepr.insertItem(self.__tr("Single(ref)"))
        self.cmbRepr.insertItem(self.__tr("Usual(Single + medium)"))
        self.cmbRepr.insertItem(self.__tr("Metals(Usual + high)"))
        self.cmbRepr.insertItem(self.__tr("All(Metals + low)"))"""
        for _ in self.m__repr:
            self.cmbRepr.insertItem(self.__tr(_))
        self.cmbRepr.setCurrentItem(self.m__iRepr)
        self.ckbByID.setText(self.__tr("By &ID..."))
        self.gbID.setTitle(QString.null)
        self.bgID.setTitle(QString.null)
        self.rbMOL_ID.setText(self.__tr("&MOL_ID"))
        self.rbZINC_ID.setText(self.__tr("&ZINC_ID"))
        self.twSearch.changeTab(self.tab,self.__tr("Search"))
        self.gbTable.setTitle(QString.null)
        self.tblResult.horizontalHeader().setLabel(self.COL__MOL_ID, self.__tr("MOL ID"))
        self.tblResult.horizontalHeader().setLabel(self.COL__SUBSET, self.__tr("Subset"))
        self.tblResult.horizontalHeader().setLabel(self.COL__ZINC_ID, self.__tr("ZINC ID"))
        self.tblResult.horizontalHeader().setLabel(self.COL__REPR, self.__tr("Repr"))
        self.tblResult.horizontalHeader().setLabel(self.COL__SERIAL, self.__tr("Serial"))
        self.tblResult.horizontalHeader().setLabel(self.COL__MWT, self.__tr("MWT"))
        self.tblResult.horizontalHeader().setLabel(self.COL__XLOGP, self.__tr("xLogP"))
        self.tblResult.horizontalHeader().setLabel(self.COL__AP_DESOLV, self.__tr("ap_Desov."))
        self.tblResult.horizontalHeader().setLabel(self.COL__P_DESOLV, self.__tr("p_Desolv."))
        self.tblResult.horizontalHeader().setLabel(self.COL__HBD, self.__tr("HBD"))
        self.tblResult.horizontalHeader().setLabel(self.COL__HBA, self.__tr("HBA"))
        self.tblResult.horizontalHeader().setLabel(self.COL__TPSA, self.__tr("tPSA"))
        self.tblResult.horizontalHeader().setLabel(self.COL__CHARGE, self.__tr("Charge"))
        self.tblResult.horizontalHeader().setLabel(self.COL__NRB, self.__tr("NRB"))
        self.tblResult.horizontalHeader().setLabel(self.COL__SMILES, self.__tr("SMILES"))
        self.tblResult.horizontalHeader().setLabel(self.COL__NAME, self.__tr("Name"))
        self.ckbExport.setText(self.__tr("E&xport..."))
        self.gbExport.setTitle(self.__tr("Export \".mol2\""))
        self.textLabel6.setText(self.__tr("Root Pathname"))
        self.textLabel5.setText(self.__tr("S&ub Pathname"))
        self.textLabel5.setBuddy(self.cmbSubPath)
        self.textLabel7.setBuddy(self.cmbBaseFilename)
        self.cmbSubPath.clear()
        """self.cmbSubPath.insertItem(self.__tr("MOL#"))
        self.cmbSubPath.insertItem(self.__tr("None"))
        self.cmbSubPath.insertItem(self.__tr("%"))"""
        for _ in self.m__subPath:
            self.cmbSubPath.insertItem(self.__tr(_))
        self.cmbSubPath.setCurrentItem(self.m__iSubPath)
        self.textLabel7.setText(self.__tr("&Base Filename"))
        self.cmbBaseFilename.clear()
        """self.cmbBaseFilename.insertItem(self.__tr("ZINC#-repr.#"))
        self.cmbBaseFilename.insertItem(self.__tr("ZINC#-%-#"))
        self.cmbBaseFilename.insertItem(self.__tr("ZINC%-repr-%"))
        self.cmbBaseFilename.insertItem(self.__tr("ZINC%-%-%"))
        self.cmbBaseFilename.insertItem(self.__tr("MOL#-ZINC#-Repr.#"))
        self.cmbBaseFilename.insertItem(self.__tr("MOL%-ZINC#-Repr.#"))"""
        for _ in self.m__baseFilename:
            self.cmbBaseFilename.insertItem(self.__tr(_))
        self.cmbBaseFilename.setCurrentItem(self.m__iBaseFilename)
        self.pbBrowse.setText(self.__tr("Set &Path..."))
        self.pbExport.setText(self.__tr("&Export"))
        self.tlSubFullPathname.setText(self.__tr("Sub-full Pathname"))
        self.twSearch.changeTab(self.tab_2,self.__tr("Result"))
        self.pbClearAll.setText(self.__tr("&Clear All"))
#       self.pbSave.setText(self.__tr("Save"))
        self.twSearch.changeTab(self.TabPage,self.__tr("Log"))


    def fnSubset(self):
#       print "SearchForm.fnSubset(): Not implemented yet"
        self.m__iSubset = self.cmbSubset.currentItem()

    def fnRepr(self):
#       print "SearchForm.fnRepr(): Not implemented yet"
        self.m__iRepr = self.cmbRepr.currentItem()

    def fnByID(self):
#       print "SearchForm.fnByID(): Not implemented yet"
        self.m__byID = self.ckbByID.isChecked()
        self.setWidgetStatesInSearchTab()

    def fnByMOL_ID(self):
#       print "SearchForm.fnByMOL_ID(): Not implemented yet"
        isChecked = self.rbMOL_ID.isChecked()
        self.m__byMOL_ID = isChecked
        self.m__byZINC_ID = not isChecked
        self.setWidgetStatesInSearchTab()

    def fnByZINC_ID(self):
#       print "SearchForm.fnByZINC_ID(): Not implemented yet"
        isChecked = self.rbZINC_ID.isChecked()
        self.m__byMOL_ID = not isChecked
        self.m__byZINC_ID = isChecked
        self.setWidgetStatesInSearchTab()

    def fnSearch(self):
#       print "SearchForm.fnSearch(): Not implemented yet"
        if self.pbSearch.text() == '&Abort':
            self.mf__abort(self.pbSearch, self.ABORT__STOP)
            return

        self.m__ID = str(self.leID.text()).strip()

        self.m__minxLogP = str(self.leMinxLogP.text()).strip()
        self.m__maxxLogP = str(self.leMaxxLogP.text()).strip()
        self.m__minCharge = str(self.leMinCharge.text()).strip()
        self.m__maxCharge = str(self.leMaxCharge.text()).strip()
        self.m__minNRB = str(self.leMinNRB.text()).strip()
        self.m__maxNRB = str(self.leMaxNRB.text()).strip()

        # Setup query.
        whereCondition = 'B.MOL_ID = A.MOL_ID AND C.MOL_ID = A.MOL_ID'
        if self.m__byID:
            if self.m__byMOL_ID:
                ########
                _IDs = self.m__ID.split()
                IDs = []    # Valid IDs
                for _ in _IDs:
                    try:
                        i = int(_.upper().lstrip('MOL0'))
                        IDs.append(str(i))  # For join() below
                    except:
                        self.teLog.append('|%s|: Cannot convert to integer: Ignored' % _)
#               print IDs
                ########
                if len(IDs):
                    whereCondition += ' AND B.MOL_ID IN (%s)' % ', '.join(IDs)
            else:   # m__byZINC_ID
                ########
                _IDs = self.m__ID.split()
                IDs = []    # Valid IDs
                for _ in _IDs:
                    try:
                        i = int(_.upper().lstrip('ZINC0'))
                        IDs.append(str(i))  # For join() below
                    except:
                        self.teLog.append('|%s|: Cannot convert to integer: Ignored' % _)
#               print IDs
                ########
                if len(IDs):
                    whereCondition += ' AND B.ZINC_ID IN (%s)' % ', '.join(IDs)

                if self.m__iRepr == self.REPR__REF or self.m__iRepr == self.REPR__SINGLE:
                    whereCondition += " AND B.Repr = 'ref'"
                elif self.m__iRepr == self.REPR__MID:
                    whereCondition += " AND B.Repr = 'mid'"
                elif self.m__iRepr == self.REPR__HI:
                    whereCondition += " AND B.Repr = 'hi'"
                elif self.m__iRepr == self.REPR__LO:
                    whereCondition += " AND B.Repr = 'lo'"
                elif self.m__iRepr == self.REPR__USUAL:
                    whereCondition += " AND (B.Repr = 'ref' OR B.Repr = 'mid')"
                elif self.m__iRepr == self.REPR__METAL:
                    whereCondition += " AND B.Repr != 'lo'"
                else:   # REPR__ALL
                    # No constraint for representative
                    pass
        else:
            if self.m__iSubset == self.SUBSET__ANY:
                pass
            elif self.m__iSubset == self.SUBSET__NCI1990:
                whereCondition += " AND A.Subset = 'NCI1990'"
            elif self.m__iSubset == self.SUBSET__CF:
                whereCondition += " AND A.Subset = 'CF'"
            else:   # SUBSET__BOTH
                whereCondition += " AND A.Subset = 'NCI1990' AND A.Subset = 'CF'"

            if self.m__iRepr == self.REPR__REF or self.m__iRepr == self.REPR__SINGLE:
                whereCondition += " AND B.Repr = 'ref'"
            elif self.m__iRepr == self.REPR__MID:
                whereCondition += " AND B.Repr = 'mid'"
            elif self.m__iRepr == self.REPR__HI:
                whereCondition += " AND B.Repr = 'hi'"
            elif self.m__iRepr == self.REPR__LO:
                whereCondition += " AND B.Repr = 'lo'"
            elif self.m__iRepr == self.REPR__USUAL:
                whereCondition += " AND (B.Repr = 'ref' OR B.Repr = 'mid')"
            elif self.m__iRepr == self.REPR__METAL:
                whereCondition += " AND B.Repr != 'lo'"
            else:   # REPR__ALL
                # No constraint for representative
                pass

            if self.m__minxLogP != '':
                whereCondition += " AND B.xLogP >= %s" % self.m__minxLogP
            if self.m__maxxLogP != '':
                whereCondition += " AND B.xLogP <= %s" % self.m__maxxLogP
            if self.m__minCharge != '':
                whereCondition += " AND B.Charge >= %s" % self.m__minCharge
            if self.m__maxCharge != '':
                whereCondition += " AND B.Charge <= %s" % self.m__maxCharge
            if self.m__minNRB != '':
                whereCondition += " AND B.NRB >= %s" % self.m__minNRB
            if self.m__maxNRB != '':
                whereCondition += " AND B.NRB <= %s" % self.m__maxNRB

        self.teLog.append('WHERE:= |%s|' % whereCondition)
        query = 'SELECT A.MOL_ID, Subset, ZINC_ID, Repr, Serial, MWT, xLogP, ap_desolvation, p_desolvation, HBD, HBA, tPSA, Charge, NRB, SMILES, Name FROM tblSubset AS A, tblProperty AS B, tblMOL2File AS C WHERE %s ORDER BY Subset, ZINC_ID, Repr, Serial' % whereCondition
        self.teLog.append('Query:= |%s|' % query)

        if self.m__isExportThreadRunning:
            self.teLog.append('Export thread is running; wait for the thread to terminate')
            return
        if self.m__isPopulateTableThreadRunning:
            self.teLog.append('Populating Table thread is already running; retry later')
            return
        self.m__isPopulateTableThreadRunning = True
        self.pbClearAll.setEnabled(False)

        self.m__bSearchDone = False
        self.setWidgetStatesInResultTab()

        self.m__whereCondition = whereCondition
        self.m__populateTableThread = PopulateTableThread(self, query)
        self.pbSearch.setText(self.__tr('&Abort'))
        self.prbSearch.reset()
        self.m__abortValue = self.ABORT__CONTINUE
        self.m__populateTableThread.start()

    def fnExportOpen(self):
#       print "SearchForm.fnExportOpen(): Not implemented yet"
        self.m__bExportOpen = self.ckbExport.isChecked()
        self.setWidgetStatesInResultTab()

    def fnBrowse(self):
#       print "SearchForm.fnBrowse(): Not implemented yet"
        rootPath = QFileDialog.getExistingDirectory('', None, '', 'Set Root Directory')
        if rootPath:
            self.m__rootPath = str(rootPath)
            self.setWidgetStatesInResultTab()

    def fnSubPathname(self):
#       print "SearchForm.fnSubPathname(): Not implemented yet"
        self.m__iSubPath = self.cmbSubPath.currentItem()
        self.setSubFullPathname()

    def fnBaseFilename(self):
#       print "SearchForm.fnBaseFilename(): Not implemented yet"
        self.m__iBaseFilename = self.cmbBaseFilename.currentItem()
        self.setSubFullPathname()

    def fnExport(self):
#       print "SearchForm.fnExport(): Not implemented yet"
        if self.pbExport.text() == '&Abort':
            self.mf__abort(self.pbExport, self.ABORT__STOP)
            return

        if not os.path.exists(self.m__rootPath):
            if QMessageBox.question(
                None,
                self.__tr('Make a directory'),
                self.__tr('|%s|: Does not exist: Make a directory?' % self.m__rootPath),
                QMessageBox.Yes | QMessageBox.Default,
                QMessageBox.No,
                QMessageBox.Cancel | QMessageBox.Escape
            ) != QMessageBox.Yes:
                return
            os.mkdir(self.m__rootPath, 0755)

        query = 'SELECT C.MOL_ID, ZINC_ID, Repr, Serial, MD5SUM, Content FROM tblSubset AS A, tblProperty AS B, tblMOL2File AS C WHERE %s ORDER BY C.MOL_ID' % self.m__whereCondition
        self.teLog.append('Query:= |%s|' % query)

        if self.m__isExportThreadRunning:
            self.teLog.append('Export thread is already running; retry later')
            return
        self.m__isExportThreadRunning = True
        self.pbSearch.setEnabled(False)
        self.pbClearAll.setEnabled(False)

        self.m__exportThread = ExportThread(self, query)
        self.pbExport.setText(self.__tr('&Abort'))
        self.prbExport.reset()
        self.m__abortValue = self.ABORT__CONTINUE
        self.m__exportThread.start()


    def fnClearAll(self):
#       print "SearchForm.fnClearAll(): Not implemented yet"
        self.teLog.clear()


    def __tr(self,s,c = None):
        return qApp.translate("SearchForm",s,c)

if __name__ == "__main__":
    a = QApplication(sys.argv)
    QObject.connect(a,SIGNAL("lastWindowClosed()"),a,SLOT("quit()"))
    w = searchForm(sys.argv)
    a.setMainWidget(w)
    w.show()
    a.exec_loop()
    NEventPending = 0
    while a.hasPendingEvents():
        NEventPending += 1
        a.processEvents()
    if w.m__bTrace:
        print '# of event pending = %d' % NEventPending
