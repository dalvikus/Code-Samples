PYTHON_PREFIX = /usr/local/Package/Python-2.5.2
PYTHON_CCFLAGS = -I$(PYTHON_PREFIX)/include/python2.5
PYTHON_LDFLAGS = -L$(PYTHON_PREFIX)/lib/python2.5/config -lpython2.5 -ldl -lutil

CAgentThread.o: CAgentThread.c++ CAgentThread.h \
  CThread.o
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) CAgentThread.c++

CAgentSlot.o: CAgentSlot.c++ CAgentSlot.h \
  CJobDown.o
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) CAgentSlot.c++

CBuffer.o: CBuffer.c++ CBuffer.h
	g++ -Wall -c $(CCFLAGS) CBuffer.c++

CFile.o: CFile.c++ CFile.h \
  CBuffer.h
	g++ -Wall -c $(CCFLAGS) CFile.c++

CJobDown.o: CJobDown.c++ CJobDown.h \
  CBuffer.h CPacket.h
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) CJobDown.c++

CJobDownPacket.o: CJobDownPacket.c++ CJobDownPacket.h \
  CBuffer.h CPacket.h
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) CJobDownPacket.c++

CPacket.o: CPacket.c++ CPacket.h \
  CZip.h
	g++ -Wall -c $(CCFLAGS) CPacket.c++

CPacketList.o: CPacketList.c++ CPacketList.h CPacket.h \
  CBuffer.h
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) CPacketList.c++

CPythonEngine.o: CPythonEngine.c++ CPythonEngine.h \
  CBuffer.h \
  CFile.h \
  CPythonWrapper.h \
  CWorkerModule.h
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) CPythonEngine.c++

CPythonWrapper.o: CPythonWrapper.c++ CPythonWrapper.h eprintf.h
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) CPythonWrapper.c++

CQueue.o: CQueue.c++ CQueue.h
	g++ -Wall -c $(CCFLAGS) CQueue.c++

CSocket.o: CSocket.c++ CSocket.h
	g++ -Wall -c $(CCFLAGS) CSocket.c++

CSocketOUT.o: CSocketOUT.c++ CSocketOUT.h \
  CQueue.o
	g++ -Wall -c $(CCFLAGS) CSocketOUT.c++

CDatabaseIN.o: CDatabaseIN.c++ CDatabaseIN.h \
  CQueue.o
	g++ -Wall -c $(CCFLAGS) CDatabaseIN.c++

CThread.o: CThread.c++ CThread.h
	g++ -Wall -c $(CCFLAGS) CThread.c++

CQueueSocketThread.o: CQueueSocketThread.c++ CQueueSocketThread.h \
  CSocketOUT.o CThread.o
	g++ -Wall -c $(CCFLAGS) CQueueSocketThread.c++

CQueueDatabaseThread.o: CQueueDatabaseThread.c++ CQueueDatabaseThread.h \
  CDatabaseIN.o CThread.o
	g++ -Wall -c $(CCFLAGS) CQueueDatabaseThread.c++

CMgmtSrv.o: CMgmtSrv.c++ CMgmtSrv.h \
  CBuffer.h \
  CFile.h \
  CPythonEngine.h \
  CPythonWrapper.h \
  CSocket.h \
  CSocketOUT.h \
  CWorkerModule.h
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) CMgmtSrv.c++

CWorker.o: CWorker.c++ CWorker.h \
  CBuffer.h \
  CFile.h \
  CPythonEngine.h \
  CPythonWrapper.h \
  CSocket.h \
  CSocketOUT.h \
  CWorkerModule.h
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) CWorker.c++

CWorkerModule.o: CWorkerModule.c++ CWorkerModule.h
	g++ -Wall -c $(CCFLAGS) CWorkerModule.c++

CZip.o: CZip.c++ CZip.h \
  CBuffer.h
	g++ -Wall -c $(CCFLAGS) CZip.c++

eprintf.o: eprintf.c eprintf.h
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) eprintf.c

gmCryptAPICore.o: gmCryptAPICore.c gmCryptAPICore.h
	g++ -Wall -c $(CCFLAGS) gmCryptAPICore.c


workerTypeI.o: workerTypeI.c++ \
  CBuffer.h \
  CFile.h \
  CPythonEngine.h \
  CPythonWrapper.h \
  CWorker.h \
  CWorkerModule.h
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) workerTypeI.c++

workerTypeII.o: workerTypeII.c++ \
  CBuffer.h \
  CFile.h \
  CPythonEngine.h \
  CPythonWrapper.h \
  CWorkerModule.h \
  CWorker.h
	g++ -Wall -c $(CCFLAGS) $(PYTHON_CCFLAGS) workerTypeI.c++

workerTypeIDirectExample: workerTypeIDirectExample.c++ \
  CBuffer.o CFile.o workerTypeI.o
	g++ -Wall $(CCFLAGS) -o workerTypeIDirectExample $(PYTHON_CCFLAGS) workerTypeIDirectExample.c++ \
        CBuffer.o \
        CFile.o \
        workerTypeI.o \
        $(PYTHON_LDFLAGS)


libworkerTypeI.so: workerTypeI.c++ \
  CBuffer.h \
  CFile.h \
  CPythonEngine.h \
  CPythonWrapper.h \
  CWorker.h \
  CWorkerModule.h
	g++ -Wall $(CCFLAGS) -o libworkerTypeI.so $(PYTHON_CCFLAGS) workerTypeI.c++ \
        -shared -Wl,-soname="libworkerTypeI.so",$(PYTHON_LDFLAGS)

libworkerTypeII.so: workerTypeII.c++ \
  CBuffer.h \
  CFile.h \
  CPythonEngine.h \
  CPythonWrapper.h \
  CWorker.h \
  CWorkerModule.h
	g++ -Wall $(CCFLAGS) -o libworkerTypeII.so $(PYTHON_CCFLAGS) workerTypeII.c++ \
        -shared -Wl,-soname="libworkerTypeII.so",$(PYTHON_LDFLAGS)

workerTypeIIModuleExample: workerTypeIIModuleExample.c++ \
  CFile.h \
  CBuffer.h \
  CPythonEngine.h \
  CPythonWrapper.h \
  CWorker.h \
  CWorkerModule.h \
    \
  CBuffer.o \
  CFile.o \
  CWorkerModule.o \
    \
  libworkerTypeII.so
	g++ -Wall $(CCFLAGS) -o workerTypeIIModuleExample -rdynamic $(PYTHON_CCFLAGS) workerTypeIIModuleExample.c++ \
        CBuffer.o \
        CFile.o \
        CWorkerModule.o \
        $(PYTHON_LDFLAGS)


ms: ms.c++ \
  CAgentThread.o CAgentSlot.o \
  CBuffer.o \
  CFile.o \
  CJobDown.o \
  CJobDownPacket.o \
  CPacket.o CPacketList.o \
  CPythonEngine.o \
  CPythonWrapper.o \
  CSocket.o \
  CSocketOUT.o \
  CMgmtSrv.o \
  CWorkerModule.o \
  CQueueDatabaseThread.o \
  CZip.o \
  eprintf.o \
    \
  libworkerTypeI.so \
  libworkerTypeII.so
	g++ -Wall $(CCFLAGS) -rdynamic -o ms $(PYTHON_CCFLAGS) ms.c++ \
        CAgentThread.o CAgentSlot.o \
        eprintf.o CQueue.o CSocket.o CSocketOUT.o \
        CThread.o CDatabaseIN.o CQueueDatabaseThread.o \
        CBuffer.o CFile.o CWorkerModule.o \
        CPacket.o CPacketList.o CZip.o \
        CJobDown.o CJobDownPacket.o \
        CPythonEngine.o CPythonWrapper.o \
        CMgmtSrv.o \
        $(PYTHON_LDFLAGS) -lz -lrt

agent: agent.c++ \
  CBuffer.o \
  CFile.o \
  CJobDown.o \
  CJobDownPacket.o \
  CPacket.o \
  CPacketList.o \
  CPythonEngine.o \
  CPythonWrapper.o \
  CQueueSocketThread.o \
  CSocket.o \
  CSocketOUT.o \
  CWorker.o \
  CWorkerModule.o \
  CZip.o \
  eprintf.o \
  gmCryptAPICore.o \
    \
  libworkerTypeI.so \
  libworkerTypeII.so
	g++ -Wall $(CCFLAGS) -rdynamic -o agent $(PYTHON_CCFLAGS) agent.c++ \
        eprintf.o gmCryptAPICore.o CQueue.o CSocket.o CSocketOUT.o \
        CThread.o CQueueSocketThread.o \
        CBuffer.o CFile.o CWorkerModule.o \
        CZip.o CPacket.o CPacketList.o \
        CJobDown.o CJobDownPacket.o \
        CPythonEngine.o CPythonWrapper.o \
        CWorker.o \
        $(PYTHON_LDFLAGS) -lz -lcrypto -lrt

clean:
	rm -f *.out *.o *.d workerTypeIDirectExample *.so workerTypeIIModuleExample agent agent_* update.sh ms WorkerModule/*
