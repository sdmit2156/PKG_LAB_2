#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QSize>
#include <QList>
#include <QTableWidget>
#include <QImageReader>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void chooseAFolder();
    void chooseAFile();
    void startToAnalize();
    void prevPage();
    void nextPage();


private:
    Ui::MainWindow *ui;
    QString selectedFolder;
    QString selectedFile;
    struct FileInfo
    {
        QString fileName;
        QSize imageSize;
        int dpiX;
        int dpiY;
        int depth;
        QString compression;
    };
    void updateTable();
    QList<FileInfo> fileInfoList;
    int itemsPerPage;
    int currentPage;
    bool isAnalyzeDone = false;

};
#endif // MAINWINDOW_H
