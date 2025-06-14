#include "mesh.h"

const qreal unit = 1;
const qreal unitH = unit / 2;
const qreal unitV = unit * std::sqrt(3) / 2;

InputMesh buildMesh(qint32 sizeH, qint32 sizeV)
{
    InputMesh inputMesh;

    auto index = [&sizeH](qint32 h, qint32 v) { return h + (v * sizeH); };

    // points
    for (qint32 v = 0; v < sizeV; v++) {
        for (qint32 h = 0; h < sizeH; h++) {
            qreal shiftH = v % 2 == 0 ? unitH : 0;
            inputMesh.pointSeq.append(Point2D{h * unit + shiftH, v * unitV});
        }
    }

    // fixes on horizontal borders
    for (qint32 i = 0; i < sizeH; i++) {
        inputMesh.fixMap.insert(index(i, 0), Fix2D{true, true});
        inputMesh.fixMap.insert(index(i, sizeV - 1), Fix2D{true, true});
    }

    // fixes on vertical borders
    for (qint32 i = 0; i < sizeV; i++) {
        inputMesh.fixMap.insert(index(0, i), Fix2D{true, true});
        inputMesh.fixMap.insert(index(sizeH - 1, i), Fix2D{true, true});
    }

    // horizontal elements
    for (qint32 v = 0; v < sizeV; v++) {
        for (qint32 h = 0; h < sizeH - 1; h++) {
            inputMesh.elementSeq.append(Element{index(h, v), index(h + 1, v)});
        }
    }

    // vertical elements
    for (qint32 h = 0; h < sizeH; h++) {
        for (qint32 v = 0; v < sizeV - 1; v++) {
            inputMesh.elementSeq.append(Element{index(h, v), index(h, v + 1)});
        }
    }

    // diagonal elements
    for (qint32 h = 0; h < sizeH - 1; h++) {
        for (qint32 v = 0; v < sizeV - 1; v++) {
            if (v % 2 == 0) {
                inputMesh.elementSeq.append(Element{index(h, v), index(h + 1, v + 1)});
            } else {
                inputMesh.elementSeq.append(Element{index(h + 1, v), index(h, v + 1)});
            }
        }
    }

    return inputMesh;
}

void applyStarForce(InputMesh &inputMesh, qint32 point, qint32 length, qint32 sizeH, qint32 sizeV)
{
    struct
    {
        qint32 i;
        Point2D p;
    } vectorSeq[]{{point - 1, Point2D{-unit, 0}},
                  {point + 1, Point2D{unit, 0}},
                  {point - sizeH + 1, Point2D{unitH, -unitV}},
                  {point - sizeH, Point2D{-unitH, -unitV}},
                  {point + sizeH + 1, Point2D{unitH, unitV}},
                  {point + sizeH, Point2D{-unitH, unitV}}};
    for (auto &v : vectorSeq) {
        inputMesh.forceMap.insert(v.i, v.p * length);
    }
}
