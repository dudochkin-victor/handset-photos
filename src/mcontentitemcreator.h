/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOSCONTENTITEMCREATOR_H
#define PHOTOSCONTENTITEMCREATOR_H

#include <QModelIndex>
#include <MContentItem>
#include <MAbstractCellCreator>
#include <MWidget>
#include <MAction>
#include <QSizeF>

#include "mplistmodel.h"
#include "abstractcellprocessor.h"
#include "mcontentitemex.h"


class MContentItemCreator : public MAbstractCellCreator<MContentItemEx>
{
public:
    MContentItemCreator(MContentItemEx::ContentItemStyle style, int _id = 0) : itemStyle(MContentItemEx::TwoIconsTwoWidgets),
        cellProcessor(NULL), id(_id)
    {
        itemStyle = style;
        size = MAbstractCellCreator<MContentItemEx>::cellSize();

    }

    virtual ~MContentItemCreator()
    {
        if (cellProcessor)
            delete cellProcessor;
    }

    //override
    void setCellViewType(const QString &viewType) {
        cellViewType = viewType;
        MAbstractCellCreator<MContentItemEx>::setCellViewType(viewType);
        updateSize();
    }

    void setCellObjectName(const QString &objectName) {
        cellObjectName = objectName;
        MAbstractCellCreator<MContentItemEx>::setCellObjectName(objectName);
        updateSize();
    }
/*
    void setOddCellObjectName(const QString &objectName) {
        oddCellObjectName = objectName;
        //MAbstractCellCreator<MContentItemEx>::setCellObjectName(objectName);
        updateSize();
    }

    void setEvenCellObjectName(const QString &objectName) {
        evenCellObjectName = objectName;
        //MAbstractCellCreator<MContentItemEx>::setCellObjectName(objectName);
        updateSize();
    }
*/

    //override this for specifying Style to SingleIcon
    virtual MWidget *createCell(const QModelIndex &index, MWidgetRecycler &recycler) const {
        recycler.setMaxItemsPerClass(0);
        MContentItemEx *cell = dynamic_cast<MContentItemEx *>(recycler.take(MContentItem::staticMetaObject.className()));
        if (cell == NULL) {
            cell = new MContentItemEx(itemStyle, NULL, id);

            cell->index = index;
            if (!cellViewType.isEmpty())
                cell->setViewType(cellViewType);
            if (!cellObjectName.isEmpty())
                cell->setObjectName(cellObjectName);

            if (cellProcessor)
                cellProcessor->processCell(cell);

        }

        updateCell(index, cell);
        return cell;
    }

    void updateCell(const QModelIndex &index, MWidget *cell) const;

 //   void setColumns(int columns) {
 //       Q_ASSERT(columns > 0);
 //       amountOfColumns = columns;
 //   }

    void setCellProcessor(AbstractCellProcessor *cellP) {
        cellProcessor = cellP;
    }

    virtual QSizeF cellSize() const {
         return size;
    }

private:
    void updateSize() {
        MContentItemEx *cell = new MContentItemEx(itemStyle, NULL, id);
        if (!cellViewType.isEmpty())
            cell->setViewType(cellViewType);
        if (!cellObjectName.isEmpty())
            cell->setObjectName(cellObjectName);
        size = cell->effectiveSizeHint(Qt::PreferredSize);
        delete cell;
        qDebug() << "update cell Size to" << size;
    }

   QSizeF size;
   QString cellViewType;
   QString cellObjectName;
   MContentItemEx::ContentItemStyle itemStyle;
   AbstractCellProcessor *cellProcessor;    //A chance to process cell after which has been created

   int id; //identify the cell is for photo or album
};



#endif // PHOTOSCONTENTITEMCREATOR_H

