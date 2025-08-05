#pragma once

#include <QListView>

class ListViewCustom : public QListView
{
public:
    ListViewCustom(QWidget* parent = nullptr);

    QSize sizeHint() const override;

protected:
    void resizeEvent(QResizeEvent* e) override;

private:
    int mWidth;
};
