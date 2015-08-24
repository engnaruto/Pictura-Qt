#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QtGui;
class QMenu;
class QScrollArea;
class QScrollBar;
class QRubberBand;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
  //  MainWindow();
    bool loadFile(const QString &);

    ~MainWindow();

private slots:
    void on_actionSave_triggered();

//    void print();
   // void fitToWindow();
    void normalSize();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


    void on_actionRotate_Anticlockwise_triggered();

    void on_actionOpen_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionZoom_in_triggered();

    void on_actionZoom_out_triggered();

    void on_actionCrop_triggered();

    void on_actionReset_triggered();

    void on_actionRotate_Clockwise_triggered();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    void createToolbar();
    QLabel *imageLabel;
    QLabel *imageLabel_reset;
  //  QScrollArea *scrollArea;
    double scaleFactor;
    int stackLength;

    QStack<QImage> * stack;
    QStack<QImage> * redostack;

#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

 /*
    QAction *openAct;
    QAction *saveAct;
    QAction *redoAct;
    QAction *undoAct;
    QAction *printAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *cropInAct;
    QAction *resetAct;
    QAction *rotateAct;
    QAction *antirotateAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
*/
    QAction *fitToWindowAct;


    QRubberBand *rubberBand;
    QPoint MyPoint;

    bool isCrop;
    bool isScroll;
    bool isInToolBar;

    QPoint a;
    QPoint b;



   /* QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *rotateMenu;
    QMenu *helpMenu;

*/
};


//! [0]

#endif
