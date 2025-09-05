#include "exportworker.h"

void ExportWorker::expol() {
    bool ok = maker->writeToFile(outFile);
    if(!ok)
      emit exportFail(maker->failReason());
    emit exportDone();
}
