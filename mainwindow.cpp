#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
using namespace std;

typedef std::vector<std::string> stringvec;
typedef std::map<std::string, int>::iterator it_type;

int get_number_from_string(std::string s)
{
    int a;
    size_t begin = s.find_first_of("0123456789");
    size_t end = s.find_last_of("0123456789");
    string num = s.substr(begin, end - begin + 1);
    a = atoi(num.c_str());
    return a;
}

int get_cluster_size ()
{
    system("fsutil fsinfo ntfsinfo c: | findstr /all \"Bytes Per Cluster\" > ttt.txt");
    string line;
    int cluser_size;
    ifstream myfile ("ttt.txt");
    if (myfile.is_open())
    {
        while ( getline (myfile, line) )
        {
            if (line.find("Bytes Per Cluster") != std::string::npos)
            {
                cluser_size = get_number_from_string(line);
                return cluser_size;
            }
        }
        myfile.close();
        return -1;
      }

      else
        cout << "Unable to open file";
        return -1;
}

void getDir(const char* d, vector<string> & f)
{
    FILE* pipe =  NULL;
    string pCmd = "for %F in ("+  string("*") +") do @echo %F %~zF "; //d
    char buf[256];

    if( NULL == (pipe = _popen(pCmd.c_str(),"rt")))
    {
        cout << "Shit" << endl;
        return;
    }
    while (!feof(pipe))
    {
        if(fgets(buf,256,pipe) != NULL)
        {
            f.push_back(string(buf));
        }
    }
    _pclose(pipe);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("QTableWidget Example");

    this->createUI(QStringList() << trUtf8("File Name and Size")
                                 << trUtf8("Cluster count")
               );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createUI(const QStringList &headers)
{
    int i = 0;
    vector<string> files;
    getDir("C:", files);
    vector<string>::const_iterator it = files.begin();
    std::map<std::string, int> m;
    while( it != files.end())
    {
        m[*it] = get_number_from_string(*it);
        it++;
    }
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setShowGrid(true);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    int cluster_size = get_cluster_size();
    for(it_type iterator = m.begin(); iterator != m.end(); iterator++) {
        ui->tableWidget->insertRow(i);
        QString q1 = QString::fromUtf8(iterator->first.c_str());
        int check = iterator->second % cluster_size;
        string cluster_count = std::to_string((check != 0) ? iterator->second / cluster_size + 1 : iterator->second / cluster_size);
        QString q2 = QString::fromUtf8(cluster_count.c_str());
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(q1));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(q2));
        i++;
    }
    ui->tableWidget->resizeColumnsToContents();
}
