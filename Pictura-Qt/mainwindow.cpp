#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif


//! [0]

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
createToolbar();
//ui->toolBar->resize(ui->toolBar->minimumWidth(), ui->toolBar->sizeHint().height());
    rubberBand = NULL;
    isCrop = 0;
    isScroll = false;

    stack = new QStack<QImage>;
    redostack = new QStack<QImage>;

    imageLabel = new QLabel;

   // imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    imageLabel_reset = new QLabel;
    imageLabel_reset->setBackgroundRole(QPalette::Base);
    imageLabel_reset->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel_reset->setScaledContents(true);

   // scrollArea = new QScrollArea;
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setWidget(imageLabel);
//    ui->scrollArea->setCursor(QCursor(Qt::OpenHandCursor));


    setCentralWidget(ui->scrollArea);

    ui->toolBar->setVisible(false);
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

    ui->actionCrop->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionReset->setEnabled(false);
    ui->actionRotate_Anticlockwise->setEnabled(false);
    ui->actionRotate_Clockwise->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionZoom_in->setEnabled(false);
    ui->actionZoom_out->setEnabled(false);

}
QScrollArea* scrollArea;
 QToolBar* mainToolBar;
  QToolBar* container;
  QFrame* frame;
void MainWindow::createToolbar(){

     frame = new QFrame(this);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

     container = new QToolBar(this);
    scrollArea = new QScrollArea;
    container->setIconSize(QSize(50,50));
        //container->setMaximumHeight(50);
            // add a bunch of items to the toolbar for testing purposes
        //    for (int i=0; i < 20; i++)
        //    {

        //                // settings.png is just a 24x24 icon that shows up as expected
        //        container->addAction(QIcon(":/Images/gallery.png"), "Test");
        //        container->addSeparator();
        //    }
        //    scrollArea->setBackgroundRole(QPalette::Dark);
        //    scrollArea->addScrollBarWidget(container, Qt::AlignCenter);

        container->addAction(ui->actionOpen);
        container->addAction(ui->actionSave);
        container->addSeparator();
        container->addAction(ui->actionUndo);
        container->addAction(ui->actionRedo);
        container->addSeparator();
        container->addAction(ui->actionReset);
        container->addSeparator();
        container->addAction(ui->actionRotate_Anticlockwise);
        container->addAction(ui->actionRotate_Clockwise);
        container->addSeparator();
        container->addAction(ui->actionZoom_in);
        container->addAction(ui->actionZoom_out);
        container->addSeparator();
        container->addAction(ui->actionCrop);
//        container->addSeparator();
//        container->addAction(ui->actionExit);

    scrollArea->setWidget(container);
    if (scrollArea->horizontalScrollBar()->isVisible()) {
        scrollArea->setMinimumHeight(100);
    }else{
        scrollArea->setMinimumHeight(80);
//        scrollArea->setMaximumHeight(60);
    }


    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(scrollArea); // add the QToolBar to the layout instead of the QScrollArea object

    frame->setLayout(layout);

  mainToolBar = new QToolBar(tr("mainToolBar"));
    mainToolBar->addWidget(frame);
    this->addToolBar(mainToolBar);
   mainToolBar->setMovable(false);
mainToolBar->setCursor(QCursor(Qt::ArrowCursor));

}


bool MainWindow::loadFile(const QString &fileName){
    QImage image(fileName);
    if (image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
        imageLabel->setPixmap(QPixmap());
        imageLabel->adjustSize();
        imageLabel_reset->setPixmap(QPixmap());
        imageLabel_reset->adjustSize();
        return false;
    }
    updateActions();
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel_reset->setPixmap(QPixmap::fromImage(image));

    scaleFactor = 1.0;

    imageLabel->adjustSize();
    imageLabel_reset->adjustSize();

    QPixmap OriginalPix(*(imageLabel->pixmap()));
    QImage newImage;
        newImage = OriginalPix.toImage();

    stack->push(newImage);
    if(stack->size() > 5)
        stack->pop_front();

   // std::cout << "sara" << std::endl;

    setWindowFilePath(fileName);
    return true;
}


void MainWindow::normalSize(){
    imageLabel->adjustSize();
    scaleFactor = 1.0;

}

void MainWindow::updateActions()
{
    ui->actionCrop->setEnabled(true);
    ui->actionRedo->setEnabled(true);
    ui->actionReset->setEnabled(true);
    ui->actionRotate_Anticlockwise->setEnabled(true);
    ui->actionRotate_Clockwise->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionUndo->setEnabled(true);
    ui->actionZoom_in->setEnabled(true);
    ui->actionZoom_out->setEnabled(true);

}


void MainWindow::scaleImage(double factor){
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    std::cout << "so: "<< scaleFactor << std::endl;

    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);


    ui->actionZoom_in->setEnabled(scaleFactor < 3.0);
    ui->actionZoom_out->setEnabled(scaleFactor > 0.333);
   // zoomInAct->setEnabled(scaleFactor < 3.0);
   // zoomOutAct->setEnabled(scaleFactor > 0.333);

}


void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor){
  scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    MyPoint = event->pos();
//    std::cout<<"AAAAAA"<<std::endl;
    if(isCrop){
//        ui->scrollArea->unsetCursor();
        ui->scrollArea->setCursor(QCursor(Qt::CrossCursor));
//        std::cout<<"ZZZZZZ"<<std::endl;
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->setGeometry(QRect(MyPoint, QSize()));
        rubberBand->show();
        show();
        isScroll = false;
        isInToolBar = false;
    }else if (ui->scrollArea->geometry().contains(event->pos())){
//        std::cout<<"XXXXXX"<<std::endl;
        isScroll = true;
        isInToolBar = false;
     ui->scrollArea->setCursor(QCursor(Qt::ClosedHandCursor));
    }else if(container->geometry().contains(event->pos())
             ||scrollArea->geometry().contains(event->pos())
             ||mainToolBar->geometry().contains(event->pos())
             ||frame->geometry().contains(event->pos())){
//        std::cout<<"CCCCCCC"<<std::endl;
        mainToolBar->setCursor(QCursor(Qt::ArrowCursor));
        isInToolBar = true;
         isScroll = false;
    }
    std::cout<<"Mouse Pressed"<<std::endl;
    std::cout<<"isScroll = "<<isScroll <<", isCrop = "<<isCrop<<", isInToolBar"<<isInToolBar<<std::endl;
}

 QPoint pt ;
QPoint diff;
void MainWindow::mouseMoveEvent(QMouseEvent *event){
    pt = event->pos();
    diff = pt-MyPoint;
//    if (isCrop) {
//       ui->scrollArea-> setCursor(QCursor(Qt::CrossCursor));
//    }else if(isScroll){
////       std::cout<<"HHHHH"<<std::endl;
//     ui->scrollArea-> setCursor(QCursor(Qt::ClosedHandCursor));
//    }else{
//      ui->scrollArea->  setCursor(QCursor(Qt::OpenHandCursor));
//    }
//    if (ui->scrollArea->geometry().contains(event->pos())){
//        if (isCrop) {
//           ui->scrollArea-> setCursor(QCursor(Qt::CrossCursor));
//        }else if(isScroll){
//         ui->scrollArea-> setCursor(QCursor(Qt::ClosedHandCursor));
//        }else{
//          ui->scrollArea->  setCursor(QCursor(Qt::OpenHandCursor));
//        }
//    }else if(isInToolBar) {
//    mainToolBar->   setCursor(QCursor(Qt::ArrowCursor));
//    }else{
//        setCursor(QCursor(Qt::ArrowCursor));
//    }

    if(isCrop){
       rubberBand->setGeometry(QRect(MyPoint, event->pos()).normalized());
    }else if(isScroll){
//    }else if(isScroll&&(ui->scrollArea->horizontalScrollBar()->isVisible()||ui->scrollArea->verticalScrollBar()->isVisible())){

        ui->scrollArea->horizontalScrollBar()->setValue(ui->scrollArea->horizontalScrollBar()->value()+diff.x());
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value()+diff.y());

        MyPoint = pt;
    }else if(isInToolBar){

      scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value()+diff.x());
      scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->value()+diff.y());


//      std::cout<<"IsInToolBar"<<std::endl;
//      std::cout<<"ScrollBars -> "<<ui->scrollArea->horizontalScrollBar()->value()<<", "<<ui->scrollArea->verticalScrollBar()->value()<<std::endl;
//      std::cout<<"Difference -> "<<diff.x()<<", "<<diff.y()<<std::endl;
//      std::cout<<"IsInToolBar"<<std::endl;



      MyPoint = pt;

    }
    std::cout<<"Mouse Moved"<<std::endl;
    std::cout<<"isScroll = "<<isScroll <<", isCrop = "<<isCrop<<", isInToolBar"<<isInToolBar<<std::endl;
}



void MainWindow::mouseReleaseEvent(QMouseEvent *event){
//    ui->scrollArea->geometry().contains()

    if(isCrop){
        updateActions();
         a = mapToGlobal(MyPoint);
         b = event->globalPos();
        /*check for opsite direction*/
          QImage copyImage;
        if(a.x() != b.x() && a.y() != b.y() && isCrop) {
               if(a.x() > b.x() && a.y() < b.y() )
               {
                   int y = a.x();
                   a.setX(b.x());
                   b.setX(y);
               }
               if(a.x() > b.x() && a.y() > b.y() )
               {
                   int x = a.x();
                   int y = a.y();
                   a.setX(b.x());
                   a.setY(b.y());
                   b.setX(x);
                   b.setY(y);
               }
               if(a.x() < b.x() && a.y() > b.y() )
               {
                   int y1 = b.y();
                   int y2 = a.y();
                   a.setY(y1);
                   b.setY(y2);
               }

            a = imageLabel->mapFromGlobal(a);
            b = imageLabel->mapFromGlobal(b);
            rubberBand->hide();
            QPixmap OriginalPix(*(imageLabel->pixmap()));
            double sx = imageLabel->rect().width();
            double sy = imageLabel->rect().height();
            sx = OriginalPix.width() / sx;
            sy = OriginalPix.height() / sy;
            a.setX(int(a.x() * sx));
            b.setX(int(b.x() * sx));
            a.setY(int(a.y() * sy));
            b.setY(int(b.y() * sy));

            QRect myRect(a, b);
            QImage newImage;
                newImage = OriginalPix.toImage();


                copyImage = newImage.copy(myRect);

                imageLabel->setPixmap(QPixmap::fromImage(copyImage));

                imageLabel->adjustSize();

                scaleFactor = 1.0;
                imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

                    imageLabel->adjustSize();


                ui->scrollArea->setBackgroundRole(QPalette::Dark);
                ui->scrollArea->setWidget(imageLabel);
                setCentralWidget(ui->scrollArea);

                rubberBand= NULL;
                isCrop = 0;
                stack->push(copyImage);
                if(stack->size() > 5)
                    stack->pop_front();

        }else{
            rubberBand->setVisible(false);
        }
   ui->scrollArea->setCursor(QCursor(Qt::OpenHandCursor));
      }else if (isScroll) {
            isScroll = false;
        ui->scrollArea->    setCursor(QCursor(Qt::OpenHandCursor));
    }else if (isInToolBar){
            isInToolBar = false;
          mainToolBar->  setCursor(QCursor(Qt::ArrowCursor));
      }else{
        setCursor(QCursor(Qt::ArrowCursor));
        }

    std::cout<<"Mouse Released"<<std::endl;
    std::cout<<"isScroll = "<<isScroll <<", isCrop = "<<isCrop<<", isInToolBar"<<isInToolBar<<std::endl;

}





MainWindow::~MainWindow(){
    delete ui;
}





void MainWindow::on_actionSave_triggered(){
    QString imagePath = QFileDialog::getSaveFileName(this,tr("Save File"),"",tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" ));
     QPixmap OriginalPix(*(imageLabel->pixmap()));
     QImage newImage;
         newImage = OriginalPix.toImage();
      newImage.save(imagePath);
}






void MainWindow::on_actionOpen_triggered(){
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}



void MainWindow::on_actionUndo_triggered(){
    if(!stack->isEmpty())
    {
         std::cout << "undo -> " << stack->size() << std::endl;
        redostack->push(stack->pop());
            ui->actionRedo->setEnabled(true);


        if(!stack->isEmpty())
        {


            imageLabel->setPixmap(QPixmap::fromImage(stack->top()));

            imageLabel->adjustSize();

            scaleFactor = 1.0;
            imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

                imageLabel->adjustSize();


            ui->scrollArea->setBackgroundRole(QPalette::Dark);
            ui->scrollArea->setWidget(imageLabel);
            setCentralWidget(ui->scrollArea);
        }
        else
        {
            ui->actionUndo->setEnabled(false);
        }
    }
    else
    {
        ui->actionUndo->setEnabled(false);
    }
}



void MainWindow::on_actionRedo_triggered(){
    if(!redostack->isEmpty())
    {
         std::cout << "redo -> " << redostack->size() << std::endl;

             imageLabel->setPixmap(QPixmap::fromImage(redostack->top()));

             stack->push(redostack->pop());
              ui->actionUndo->setEnabled(true);


            imageLabel->adjustSize();

            scaleFactor = 1.0;
            imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());



                imageLabel->adjustSize();


            ui->scrollArea->setBackgroundRole(QPalette::Dark);
            ui->scrollArea->setWidget(imageLabel);
            setCentralWidget(ui->scrollArea);
    }
    else
    {
        ui->actionRedo->setEnabled(false);
    }

}



void MainWindow::on_actionZoom_in_triggered(){
   scaleImage(1.25);
//   updateActions();
}



void MainWindow::on_actionZoom_out_triggered(){
 scaleImage(0.8);
// updateActions();
}



void MainWindow::on_actionCrop_triggered(){
     ui->scrollArea->setCursor(QCursor(Qt::CrossCursor));
   isCrop = 1;
}



void MainWindow::on_actionReset_triggered(){
    QPixmap OriginalPix(*(imageLabel_reset->pixmap()));

    QImage newImage;
        newImage = OriginalPix.toImage();

        imageLabel->setPixmap(QPixmap::fromImage(newImage));

        imageLabel->adjustSize();

        scaleFactor = 1.0;
        imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

        imageLabel->adjustSize();


        ui->scrollArea->setBackgroundRole(QPalette::Dark);
        ui->scrollArea->setWidget(imageLabel);
        setCentralWidget(ui->scrollArea);

        while(!stack->isEmpty())
        {
            stack->pop();
        }
        while(!redostack->isEmpty())
        {
            redostack->pop();
        }
        QPixmap OriginalPix1(*(imageLabel_reset->pixmap()));
        QImage newImage1;
        newImage1 = OriginalPix1.toImage();
        stack->push(newImage1);

        updateActions();


}



void MainWindow::on_actionRotate_Clockwise_triggered(){
    QPixmap OriginalPix(*(imageLabel->pixmap()));
    QTransform transform;
    QTransform trans = transform.rotate(90);
    QPixmap * trpix = new QPixmap(OriginalPix.transformed(trans));
    QImage newImage;
        newImage = trpix->toImage();

    imageLabel->setPixmap(QPixmap::fromImage(newImage));

    imageLabel->adjustSize();

    scaleFactor = 1.0;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

        imageLabel->adjustSize();


    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setWidget(imageLabel);
    setCentralWidget(ui->scrollArea);


    stack->push(newImage);
    if(stack->size() > 5)
        stack->pop_front();
    updateActions();
}



void MainWindow::on_actionRotate_Anticlockwise_triggered(){
    QPixmap OriginalPix(*(imageLabel->pixmap()));
    QTransform transform;
    QTransform trans = transform.rotate(-90);
    QPixmap * trpix = new QPixmap(OriginalPix.transformed(trans));
    QImage newImage;
        newImage = trpix->toImage();

    imageLabel->setPixmap(QPixmap::fromImage(newImage));

    imageLabel->adjustSize();

    scaleFactor = 1.0;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

        imageLabel->adjustSize();

    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setWidget(imageLabel);
    setCentralWidget(ui->scrollArea);


    stack->push(newImage);
    if(stack->size() > 5)
        stack->pop_front();

    updateActions();

}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::exit();
}
