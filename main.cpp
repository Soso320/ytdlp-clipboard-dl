#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <regex>
#include <set>
#include <string>
#include <iostream>
#include <cstdlib>

class VideoDownloaderApp : public QWidget {
    Q_OBJECT

public:
    VideoDownloaderApp(QWidget *parent = nullptr);

private slots:
    void selectDownloadPath();
    void downloadVideo();
    void showError(const QString &message);
    void clearError();

private:
    bool isValidUrl(const std::string &url);

    QTextEdit *log;
    QLabel *errorLabel;
    QString downloadPath;
    std::set<std::string> completedLinks;
};

VideoDownloaderApp::VideoDownloaderApp(QWidget *parent)
    : QWidget(parent) {
    setWindowTitle("Video Downloader");

    downloadPath = QDir::homePath();

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QVBoxLayout *logLayout = new QVBoxLayout;
    QLabel *logLabel = new QLabel("Command Log", this);
    log = new QTextEdit(this);
    log->setReadOnly(true);
    logLayout->addWidget(logLabel);
    logLayout->addWidget(log);
    mainLayout->addLayout(logLayout);

    QHBoxLayout *bottomLayout = new QHBoxLayout;

    QPushButton *pathButton = new QPushButton("Set Path", this);
    connect(pathButton, &QPushButton::clicked, this, &VideoDownloaderApp::selectDownloadPath);
    bottomLayout->addWidget(pathButton);

    QPushButton *downloadButton = new QPushButton("Download", this);
    downloadButton->setStyleSheet("background-color: green; color: white; font-size: 16px;");
    connect(downloadButton, &QPushButton::clicked, this, &VideoDownloaderApp::downloadVideo);
    bottomLayout->addWidget(downloadButton);

    mainLayout->addLayout(bottomLayout);

    errorLabel = new QLabel("", this);
    errorLabel->setStyleSheet("color: red");
    mainLayout->addWidget(errorLabel);

    setLayout(mainLayout);
}

bool VideoDownloaderApp::isValidUrl(const std::string &url) {
    const std::regex urlRegex(
        R"(^(?:http|ftp)s?://)"
        R"((?:(?:[A-Z0-9](?:[A-Z0-9-]{0,61}[A-Z0-9])?\.)+(?:[A-Z]{2,6}\.?|[A-Z0-9-]{2,}\.?)|)"
        R"(localhost|\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}|\[?[A-F0-9]*:[A-F0-9:]+\]?)"
        R"(?::\d+)?(?:/?|[/?]\S+)$)", std::regex::icase);
    return std::regex_match(url, urlRegex);
}

void VideoDownloaderApp::selectDownloadPath() {
    QString path = QFileDialog::getExistingDirectory(this, "Select Download Directory", downloadPath);
    if (!path.isEmpty()) {
        downloadPath = path;
    }
}

void VideoDownloaderApp::downloadVideo() {
    QClipboard *clipboard = QApplication::clipboard();
    QString clipboardContent = clipboard->text();

    std::string url = clipboardContent.toStdString();

    if (!isValidUrl(url)) {
        showError("The clipboard does not contain a valid URL.");
        return;
    }

    if (completedLinks.find(url) != completedLinks.end()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Duplicate Link", 
                                                                  "This link has already been downloaded. Do you want to download it again?", 
                                                                  QMessageBox::Yes | QMessageBox::No, 
                                                                  QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    log->append("Downloading: " + clipboardContent + "\n");

    QString downloadCommand = QString("yt-dlp -o \"%1/%(title)s.%(ext)s\" %2")
                                .arg(downloadPath)
                                .arg(clipboardContent);

    int result = std::system(downloadCommand.toStdString().c_str());

    if (result == 0) {
        log->append("Download completed: " + clipboardContent + "\n");
        completedLinks.insert(url);
    } else {
        log->append("Error downloading: " + QString::number(result) + "\n");
    }
}

void VideoDownloaderApp::showError(const QString &message) {
    errorLabel->setText(message);
    QTimer::singleShot(3000, this, &VideoDownloaderApp::clearError);
}

void VideoDownloaderApp::clearError() {
    errorLabel->setText("");
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    VideoDownloaderApp downloader;
    downloader.show();
    return app.exec();
}

#include "main.moc"
