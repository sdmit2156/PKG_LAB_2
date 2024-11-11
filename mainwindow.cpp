#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    currentPage = 0;
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::chooseAFolder);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::startToAnalize);
    connect(ui->backButton, &QPushButton::clicked, this,&MainWindow::prevPage);
    connect(ui->nextButton, &QPushButton::clicked, this, &MainWindow::nextPage);
    connect(ui->fileButton, &QPushButton::clicked, this, &MainWindow::chooseAFile);
    itemsPerPage = 17;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::chooseAFolder()
{

    QString folder = QFileDialog::getExistingDirectory(this, "Выберите папку");
    if(!folder.isEmpty())
    {
        selectedFolder = folder;
        ui->label->setText("Вы выбрали папку " + folder);
        ui->label->setEnabled(true);
        ui->progressBar->setEnabled(true);
        ui->progressBar->setValue(0);
        ui->pushButton_2->setEnabled(true);
        ui->fileButton->setEnabled(true);
    }
    else
    {
        qDebug() << "Папка уже выбрана. Чтобы выбрать другую, сначала сбросьте выбор.";
    }
    qDebug() << folder;
    isAnalyzeDone=false;
}

void MainWindow::chooseAFile()
{
    QString file = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Images (*.png *.xpm *.jpg *.jpeg *.bmp *.gif *.tif)");
    if(!file.isEmpty())
    {
        selectedFile = file;
        ui->label->setText("Вы выбрали файл " + file);
        ui->label->setEnabled(true);
        ui->progressBar->setEnabled(true);
        ui->progressBar->setValue(0);
        ui->pushButton_2->setEnabled(true);
    }
    else
    {
        qDebug() << "Файл уже выбран. Чтобы выбрать другой, сначала сбросьте выбор.";
    }
    qDebug() << file;
    isAnalyzeDone=false;
}


void MainWindow::startToAnalize()
{
    if(isAnalyzeDone)
    {
        return;
    }

    if (!selectedFolder.isEmpty()) {
        QDir directory(selectedFolder);
        QFileInfoList infoList = directory.entryInfoList(QDir::Files);
        ui->progressBar->setMaximum(infoList.size());
        fileInfoList.clear();
        int row = 0;
        for(const QFileInfo &infoFile: infoList)
        {
            QString fileName = infoFile.fileName();
            QString filePath = infoFile.filePath();
            QString extension = infoFile.suffix().toLower();
            qint64 fileSizeOnDisk = infoFile.size();
            if (extension == "jpg" || extension == "jpeg" ||
                extension == "png" || extension == "gif" ||
                extension == "bmp" || extension == "tif" )
            {
                QImage image(filePath);
                if(image.isNull())
                {
                    qDebug() << "Не удалось загрузить изображение:" << filePath;
                    continue;
                }
                size_t imageSizeInMemory = image.sizeInBytes();
                double compressionRatio = static_cast<double>(imageSizeInMemory) / fileSizeOnDisk;
                QSize imageSize = image.size();
                int depth = image.depth();
                int dpiX = image.dotsPerMeterX();
                int dpiY = image.dotsPerMeterY();
                QString compression = QString::number(compressionRatio);
                qDebug() << "имя файла" << fileName;
                qDebug() << "размер в пикселях" << imageSize;
                qDebug() << "глубина" << depth;
                qDebug() << "разрешение" << dpiX << " " << dpiY;
                fileInfoList.append({fileName,imageSize,dpiX,dpiY,depth,compression});
                row++;
                ui->progressBar->setValue(row);
                updateTable();
            }
        }
    } else if (!selectedFile.isEmpty()) {
        QFileInfo infoFile(selectedFile);
        QString fileName = infoFile.fileName();
        QString filePath = infoFile.filePath();
        QString extension = infoFile.suffix().toLower();
        qint64 fileSizeOnDisk = infoFile.size();
        if (extension == "jpg" || extension == "jpeg" ||
            extension == "png" || extension == "gif" ||
            extension == "bmp" || extension == "tif" )
        {
            QImage image(filePath);
            if(image.isNull())
            {
                qDebug() << "Не удалось загрузить изображение:" << filePath;
                return;
            }
            size_t imageSizeInMemory = image.sizeInBytes();
            double compressionRatio = static_cast<double>(imageSizeInMemory) / fileSizeOnDisk;
            QSize imageSize = image.size();
            int depth = image.depth();
            int dpiX = image.dotsPerMeterX();
            int dpiY = image.dotsPerMeterY();
            QString compression = QString::number(compressionRatio);
            qDebug() << "имя файла" << fileName;
            qDebug() << "размер в пикселях" << imageSize;
            qDebug() << "глубина" << depth;
            qDebug() << "разрешение" << dpiX << " " << dpiY;
            fileInfoList.append({fileName,imageSize,dpiX,dpiY,depth,compression});
            ui->progressBar->setValue(100);
            updateTable();
        }
    }

    isAnalyzeDone = true;
    QMessageBox::information(this,"Анализ завершен", "Анализ завершен успешно!");
}

void MainWindow::prevPage()
{
    if(currentPage > 0)
    {
        currentPage--;
        updateTable();
    }

}

void MainWindow::nextPage()
{
    if((currentPage + 1)* itemsPerPage < fileInfoList.size())
    {
        currentPage++;
        updateTable();

    }
}

void MainWindow::updateTable()
{
    int start = currentPage * itemsPerPage;
    int end = qMin(start + itemsPerPage, fileInfoList.size());
    ui->tableWidget->setRowCount(0);
    for(int i = start; i < end; ++i)
    {
        const FileInfo &info = fileInfoList[i];
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,0, new QTableWidgetItem(info.fileName));
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(QString("%1 на %2").arg(info.imageSize.height()).arg(info.imageSize.width())));
        ui->tableWidget->setItem(row,2,new QTableWidgetItem(QString("%1 на %2").arg(info.dpiX).arg(info.dpiY)));
        ui->tableWidget->setItem(row,3,new QTableWidgetItem(QString::number(info.depth)));
        ui->tableWidget->setItem(row,4,new QTableWidgetItem(info.compression));
    }
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
    {
        int globalRowNumber = start + i + 1;
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString::number(globalRowNumber)));
    }
}

