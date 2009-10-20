/***************************************************************************
 *   Copyright (C) 2007-2009 by Miguel Chavez Gamboa                       *
 *   miguel@lemonpos.org                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *

 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "squeeze.h"
#include "squeezeview.h"
#include "settings.h"

#include <qapplication.h>
#include <qpainter.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPrinter>

#include <kdeversion.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kconfigdialog.h>
//#include <kio/netaccess.h>
// #include <kfiledialog.h>
#include <kactioncollection.h>
#include <kaction.h>
#include <KLocale>
#include <kled.h>
#include <kstandarddirs.h>

squeeze::squeeze()
    : KXmlGuiWindow( ),
      m_view(new squeezeView(this)),
      m_printer(0)
{
    setObjectName(QLatin1String("squeeze"));
    // accept dnd
    setAcceptDrops(false);

    // tell the KXmlGuiWindow that this is indeed the main widget
    setCentralWidget(m_view);

    // then, setup our actions
    setupActions();
    //Add some widgets to status bar
    led = new KLed;
    led->off();
    statusBar()->addWidget(led); //FIXME: Que cuando se escriba algo en la barra de status, quede el LED ahi tambien.
    // add a status bar
    statusBar()->show();

    // Add typical actions and save size/toolbars/statusbar
    setupGUI();
    disableUI();
    // allow the view to change the statusbar and caption
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString&)),
            this,   SLOT(changeStatusbar(const QString&)));
    connect(m_view, SIGNAL(signalChangeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));

    connect(m_view, SIGNAL(signalDisconnected()), this, SLOT(setDisconnected()));
    connect(m_view, SIGNAL(signalConnected()), this, SLOT(setConnected()));

    connect(m_view, SIGNAL(signalShowPrefs()), SLOT(optionsPreferences()) );

    connect(m_view, SIGNAL(signalSalir() ), SLOT(salir() ));

    connect(m_view, SIGNAL(signalShowDbConfig()), this, SLOT(showDBConfigDialog()));

    loadStyle();
}

squeeze::~squeeze()
{
    delete m_printer;
}

void squeeze::loadStyle()
{
  qDebug()<<"Loading Stylesheet...";

    //Load a simple style...
    QString fileName; QString path;
    path = KStandardDirs::locate("appdata", "styles/");
    fileName = path + Settings::styleName() + "/simple.qss";
    qDebug()<<"Style file:"<<fileName;
    QFile file(fileName);
    bool op = file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    //replace fakepath to the real path..
    QString finalStyle = styleSheet.replace("[STYLE_PATH]", path + Settings::styleName() + "/");
    qApp->setStyleSheet(finalStyle);
    if (op) file.close();
}

void squeeze::setConnected()
{
  //a workaround.. just to dont modify the code in squeezeview
  setConnection(true);
}

void squeeze::setDisconnected()
{
  //a workaround.. just to dont modify the code in squeezeview
  setConnection(false);
}

void squeeze::setConnection(bool yes)
{
  if ( yes ) {
    led->on();
    enableUI();
  } else {
    led->off();
    disableUI();
  }
}

void squeeze::enableUI()
{
  qDebug()<<"Enabling Actions..";
  QAction *action = actionCollection()->action("login");
  action->setEnabled(true);
  action = actionCollection()->action("productsBrowse");
  action->setEnabled(true);
  action = actionCollection()->action("offersBrowse");
  action->setEnabled(true);
  action = actionCollection()->action("usersBrowse");
  action->setEnabled(true);
  action = actionCollection()->action("measuresBrowse");
  action->setEnabled(true);
  action = actionCollection()->action("categoriesBrowse");
  action->setEnabled(true);
  action = actionCollection()->action("balancesBrowse");
  action->setEnabled(true);
  action = actionCollection()->action("transactionsBrowse");
  action->setEnabled(true);
  action = actionCollection()->action("clientsBrowse");
  action->setEnabled(true);
  action = actionCollection()->action("doPurchase");
  action->setEnabled(true);
  action = actionCollection()->action(KStandardAction::name(KStandardAction::Preferences));
  action->setEnabled(true);
  }

void squeeze::disableUI()
{   qDebug()<<"Disabling Actions..";
  QAction *action = actionCollection()->action("login");
  action->setDisabled(true);
  action = actionCollection()->action("productsBrowse");
  action->setDisabled(true);
  action = actionCollection()->action("offersBrowse");
  action->setDisabled(true);
  action = actionCollection()->action("usersBrowse");
  action->setDisabled(true);
  action = actionCollection()->action("measuresBrowse");
  action->setDisabled(true);
  action = actionCollection()->action("categoriesBrowse");
  action->setDisabled(true);
  action = actionCollection()->action("balancesBrowse");
  action->setDisabled(true);
  action = actionCollection()->action("transactionsBrowse");
  action->setDisabled(true);
  action = actionCollection()->action("clientsBrowse");
  action->setDisabled(true);
  action = actionCollection()->action("doPurchase");
  action->setDisabled(true);
  action = actionCollection()->action(KStandardAction::name(KStandardAction::Preferences));
  action->setDisabled(true);
}

void squeeze::setupActions()
{
  KStandardAction::quit(qApp, SLOT(quit()), actionCollection());
  KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
  
  //My actions
  QAction* loginAction =  actionCollection()->addAction( "login" );
  loginAction->setText(i18n("Login"));
  loginAction->setIcon(KIcon("office-address-book"));
  loginAction->setShortcut(Qt::CTRL+Qt::Key_L);
  connect(loginAction, SIGNAL(triggered(bool)),m_view, SLOT(login()));
  
  QAction* prodBrowseAction =  actionCollection()->addAction( "productsBrowse" );
  prodBrowseAction->setText(i18n("Browse products"));
  prodBrowseAction->setIcon(KIcon("lemon-box"));
  prodBrowseAction->setShortcut(Qt::CTRL+Qt::Key_P);
  connect(prodBrowseAction, SIGNAL(triggered(bool)),m_view, SLOT(showProductsPage()));
  
  QAction* offersBrowseAction =  actionCollection()->addAction( "offersBrowse" );
  offersBrowseAction->setText(i18n("Browse offers"));
  offersBrowseAction->setIcon(KIcon("lemon-offers"));
  offersBrowseAction->setShortcut(Qt::CTRL+Qt::Key_O);
  connect(offersBrowseAction, SIGNAL(triggered(bool)),m_view, SLOT(showOffersPage()));
  
  QAction* usersBrowseAction =  actionCollection()->addAction( "usersBrowse" );
  usersBrowseAction->setText(i18n("Browse users"));
  usersBrowseAction->setIcon(KIcon("lemon-user"));
  usersBrowseAction->setShortcut(Qt::CTRL+Qt::Key_U);
  connect(usersBrowseAction, SIGNAL(triggered(bool)),m_view, SLOT(showUsersPage()));
  
  QAction* measuresBrowseAction =  actionCollection()->addAction( "measuresBrowse" );
  measuresBrowseAction->setText(i18n("Browse Measures"));
  measuresBrowseAction->setIcon(KIcon("lemon-ruler"));
  measuresBrowseAction->setShortcut(Qt::CTRL+Qt::Key_M);
  connect(measuresBrowseAction, SIGNAL(triggered(bool)),m_view, SLOT(showMeasuresPage()));
  
  QAction* categoriesBrowseAction =  actionCollection()->addAction( "categoriesBrowse" );
  categoriesBrowseAction->setText(i18n("Browse Categories"));
  categoriesBrowseAction->setIcon(KIcon("lemon-categories"));
  categoriesBrowseAction->setShortcut(Qt::CTRL+Qt::Key_C);
  connect(categoriesBrowseAction, SIGNAL(triggered(bool)),m_view, SLOT(showCategoriesPage()));
  
  QAction* balancesBrowseAction =  actionCollection()->addAction( "balancesBrowse" );
  balancesBrowseAction->setText(i18n("Browse Balances"));
  balancesBrowseAction->setIcon(KIcon("lemonbalance"));
  balancesBrowseAction->setShortcut(Qt::CTRL+Qt::Key_B);
  connect(balancesBrowseAction, SIGNAL(triggered(bool)),m_view, SLOT(showBalancesPage()));
  
  QAction* cashFlowBrowseAction =  actionCollection()->addAction( "cashFlowBrowse" );
  cashFlowBrowseAction->setText(i18n("Browse Cash Flow"));
  cashFlowBrowseAction->setIcon(KIcon("lemon-cashout"));
  cashFlowBrowseAction->setShortcut(Qt::CTRL+Qt::Key_F);
  connect(cashFlowBrowseAction, SIGNAL(triggered(bool)),m_view, SLOT(showCashFlowPage()));
  
  QAction* transactionsBrowseAction =  actionCollection()->addAction( "transactionsBrowse" );
  transactionsBrowseAction->setText(i18n("Browse Transactions"));
  transactionsBrowseAction->setIcon(KIcon("lemon-money"));//TODO:Create an icon for this...
  transactionsBrowseAction->setShortcut(Qt::CTRL+Qt::Key_T);
  connect(transactionsBrowseAction, SIGNAL(triggered(bool)),m_view, SLOT(showTransactionsPage()));
  
  QAction* clientsBrowseAction =  actionCollection()->addAction( "clientsBrowse" );
  clientsBrowseAction->setText(i18n("Browse Clients"));
  clientsBrowseAction->setIcon(KIcon("lemon-user"));//TODO:Create an icon for this...
  clientsBrowseAction->setShortcut(Qt::CTRL+Qt::Key_I);
  connect(clientsBrowseAction, SIGNAL(triggered(bool)),m_view, SLOT(showClientsPage()));
  
  QAction* purchaseAction =  actionCollection()->addAction( "doPurchase" ); //Alias Check IN
  purchaseAction->setText(i18n("Purchase"));
  purchaseAction->setIcon(KIcon("lemon-money"));//TODO:Create an icon for this...
  purchaseAction->setShortcut(Qt::Key_F2);
  connect(purchaseAction, SIGNAL(triggered(bool)),m_view, SLOT(doPurchase()));
  
  //TODO: code the check out!
  //FIXME: Check out could be confusing...
  //QAction* outAction =  actionCollection()->addAction( "doCheckOut" );
  //outAction->setText(i18n("Check Out"));
  //outAction->setIcon(KIcon("lemon-money"));//TODO:Create an icon for this...
  //outAction->setShortcut(Qt::Key_F3);
  //connect(outAction, SIGNAL(triggered(bool)),m_view, SLOT(doCheckOut()));
  
  //     connect(m_view, SIGNAL(signalConnectActions()), this, SLOT(setupWidgetActions()) );//NOTE: Para que era??? no recuerdo
  
}


void squeeze::saveProperties(KConfigGroup &config)
{
    // the 'config' object points to the session managed
    // config file.  anything you write here will be available
    // later when this app is restored

//     if (!m_view->currentURL().isNull()) {
//         config.writePathEntry("lastURL", m_view->currentURL());
//     }
}

void squeeze::readProperties(const KConfigGroup &config)
{
    // the 'config' object points to the session managed
    // config file.  this function is automatically called whenever
    // the app is being restored.  read in here whatever you wrote
    // in 'saveProperties'

//     QString url = config.readPathEntry("lastURL", QString());
//
//     if (!url.isEmpty())
//         m_view->openURL(KUrl::fromPathOrUrl(url));
}

/**This is used to get Database user,password,server to set initial config, in case the db server is remote.
So we show the config dialog, and when saved login again. It is called from main_view.login()
**/
void squeeze::showDBConfigDialog()
{
  //avoid to have 2 dialogs shown
  if ( KConfigDialog::showDialog( "settings" ) )  {
    return;
  }
  KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
  
  QWidget *dbSettingsDlg = new QWidget;
  ui_prefs_db.setupUi(dbSettingsDlg);
  dialog->addPage(dbSettingsDlg, i18n("Database"), "vcs_diff"); //kexi
  
  connect(dialog, SIGNAL(settingsChanged(QString)), m_view, SLOT(settingsChangedOnInitConfig()));
  dialog->setAttribute( Qt::WA_DeleteOnClose );
  dialog->show();
  
  
}

void squeeze::optionsPreferences()
{
    //avoid to have 2 dialogs shown
    if ( KConfigDialog::showDialog( "settings" ) )  {
        return;
    }
    KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());

    //general
    QWidget *generalSettingsDlg = new QWidget;
    ui_prefs_base.setupUi(generalSettingsDlg);
    dialog->addPage(generalSettingsDlg, i18n("General"), "configure");
    //Database
    QWidget *dbSettingsDlg = new QWidget;
    ui_prefs_db.setupUi(dbSettingsDlg);
    dialog->addPage(dbSettingsDlg, i18n("Database"), "vcs_diff"); //kexi

    connect(dialog, SIGNAL(settingsChanged(QString)), m_view, SLOT(settingsChanged()));
    //free mem by deleting the dialog on close without waiting for deletingit when the application quits
    dialog->setAttribute( Qt::WA_DeleteOnClose );

    dialog->show();
}


void squeeze::changeStatusbar(const QString& text)
{
    // display the text on the statusbar
    statusBar()->showMessage(text);
}

void squeeze::changeCaption(const QString& text)
{
    // display the text on the caption
    setCaption(text);
}

bool squeeze::queryClose()
{
  m_view->closeDB();
  return true;
}

void squeeze::salir()
{
  qApp->quit();
}

#include "squeeze.moc"
