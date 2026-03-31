#include "../input/lifewidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <vector>
#include <QSizePolicy>
#include <algorithm>
#include <QResizeEvent>
#include <QPalette>

LifeWidget::LifeWidget(int rows, int cols, QWidget *parent)
    : QWidget(parent), m_rows(rows), m_cols(cols), m_generation(0)
{
    m_grid.resize(m_rows, std::vector<bool>(m_cols, false));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(100, 100);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, parent ? parent->palette().color(QPalette::Window) : Qt::lightGray);
    setPalette(pal);
}

void LifeWidget::setCellState(int row, int col, bool alive)
{
    if (row >= 0 && row < m_rows && col >= 0 && col < m_cols) {
        m_grid[row][col] = alive;
        update();
    }
}

bool LifeWidget::cellState(int row, int col) const
{
    if (row >= 0 && row < m_rows && col >= 0 && col < m_cols) {
        return m_grid[row][col];
    }
    return false;
}

void LifeWidget::clearGrid()
{
    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            m_grid[r][c] = false;
        }
    }
    m_generation = 0;
    emit generationChanged(m_generation);
    update();
}

void LifeWidget::nextGeneration()
{
    std::vector<std::vector<bool>> nextGrid = m_grid;
    bool changed = false;

    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            int liveNeighbors = countLiveNeighbors(r, c);
            bool currentCell = m_grid[r][c];
            bool nextState = currentCell;

            if (currentCell && (liveNeighbors < 2 || liveNeighbors > 3)) {
                nextState = false;
            } else if (!currentCell && liveNeighbors == 3) {
                nextState = true;
            }

            if (nextState != currentCell) {
                nextGrid[r][c] = nextState;
                changed = true;
            }
        }
    }

    if (changed) {
         m_grid = nextGrid;
         m_generation++;
         emit generationChanged(m_generation);
         update();
    }
}

int LifeWidget::countLiveNeighbors(int row, int col)
{
    int count = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = (row + dr + m_rows) % m_rows;
            int nc = (col + dc + m_cols) % m_cols;
            if (nr >= 0 && nr < m_rows && nc >= 0 && nc < m_cols && m_grid[nr][nc]) {
                count++;
            }
        }
    }
    return count;
}

bool LifeWidget::hasHeightForWidth() const
{
    return true;
}

int LifeWidget::heightForWidth(int w) const
{
    return w;
}

QSize LifeWidget::sizeHint() const
{
    int hintSide = 400;
    return QSize(hintSide, hintSide);
}

void LifeWidget::resizeGrid(int newRows, int newCols)
{
    if (newRows > 0 && newCols > 0 && (newRows != m_rows || newCols != m_cols))
    {
        m_rows = newRows;
        m_cols = newCols;
        m_grid.assign(m_rows, std::vector<bool>(m_cols, false));
        m_generation = 0;
        emit generationChanged(m_generation);
        updateGeometry();
        update();
    }
}

void LifeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    int side = std::min(width(), height());
    int offsetX = (width() - side) / 2;
    int offsetY = (height() - side) / 2;

    qreal cellWidth = (qreal)side / m_cols;
    qreal cellHeight = (qreal)side / m_rows;

    painter.save();
    painter.translate(offsetX, offsetY);

    painter.setPen(Qt::darkGray);
    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            QRectF cellRect(c * cellWidth, r * cellHeight, cellWidth, cellHeight);
            if (m_grid[r][c]) {
                painter.fillRect(cellRect, Qt::black);
            } else {
                painter.fillRect(cellRect, Qt::white);
            }
            painter.drawRect(cellRect);
        }
    }
    painter.restore();
}

void LifeWidget::mousePressEvent(QMouseEvent *event)
{
     if (event->button() == Qt::LeftButton) {
        int side = std::min(width(), height());
        int offsetX = (width() - side) / 2;
        int offsetY = (height() - side) / 2;

        int mouseX = event->x();
        int mouseY = event->y();

        if (mouseX >= offsetX && mouseX < offsetX + side &&
            mouseY >= offsetY && mouseY < offsetY + side)
        {
            int relativeX = mouseX - offsetX;
            int relativeY = mouseY - offsetY;

            qreal cellWidth = (qreal)side / m_cols;
            qreal cellHeight = (qreal)side / m_rows;

            int col = static_cast<int>(relativeX / cellWidth);
            int row = static_cast<int>(relativeY / cellHeight);

            if (row >= 0 && row < m_rows && col >= 0 && col < m_cols) {
                m_grid[row][col] = !m_grid[row][col];
                update();
            }
        }
    } else {
         QWidget::mousePressEvent(event);
     }
}