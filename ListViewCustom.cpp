#include "ListViewCustom.hpp"

ListViewCustom::ListViewCustom(QWidget* parent)
    : QListView{parent}
    , mWidth{-1}
{
}

QSize ListViewCustom::sizeHint() const
{
    if (model()->rowCount() == 0 || mWidth == -1)
        return QSize(width(), 0);

    QSize itemSizeHint{sizeHintForColumn(0), sizeHintForRow(0)};
    int	  columns = mWidth / itemSizeHint.width();
    int	  rows = (model()->rowCount() + columns - 1) / columns;

    return QSize(mWidth, rows * itemSizeHint.height() + itemSizeHint.height() / 2);
}

void ListViewCustom::resizeEvent(QResizeEvent* e)
{
    QListView::resizeEvent(e);
    mWidth = width();
    updateGeometry();
}
