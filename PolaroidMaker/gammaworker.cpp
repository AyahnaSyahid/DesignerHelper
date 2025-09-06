#include "gammaworker.h"

#include "polaroid.h"

GammaWorker::GammaWorker(QObject *parent, QImage *img, double gm)
    : QObject(parent), target(img), gamma(gm) {}

void GammaWorker::doGamma() {
  if (!target) {
    emit done();
    return;
  }
  Polaroid::applyGamma(target, gamma);
  emit done();
}
