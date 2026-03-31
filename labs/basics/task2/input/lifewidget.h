#ifndef LIFEWIDGET_H
#define LIFEWIDGET_H

#include <QWidget>
#include <vector>
#include <QSize>

class LifeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LifeWidget(int rows = 30, int cols = 30, QWidget *parent = nullptr);
    ~LifeWidget() override = default;

    void setCellState(int row, int col, bool alive);
    bool cellState(int row, int col) const;
    void nextGeneration();
    void clearGrid();
    int generation() const { return m_generation; }
    int rows() const { return m_rows; }
    int cols() const { return m_cols; }

    void resizeGrid(int newRows, int newCols);

    QSize sizeHint() const override;
    int heightForWidth(int w) const override;
    bool hasHeightForWidth() const override;

signals:
    void generationChanged(int generation);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int countLiveNeighbors(int row, int col);

    int m_rows;
    int m_cols;
    std::vector<std::vector<bool>> m_grid;
    int m_generation;
};

#endif // LIFEWIDGET_H