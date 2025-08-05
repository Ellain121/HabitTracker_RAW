#pragma once

#include <QTableView>

class HabitDelegate;

// Same as QTableView, but most mouse events overridden
class HabTableView : public QTableView
{
public:
    HabTableView(QWidget* parent);

    void setHabitDelegate(HabitDelegate* delegate);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    HabitDelegate* mHabitDelegate;
};
