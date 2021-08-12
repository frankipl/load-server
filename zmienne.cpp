#include "zmienne.h"

Singleton::Singleton() {
    // konstruktor klasy - nie używany :))
    mainPath=QString("%1/").arg(QDir::currentPath());
    configPath=mainPath+"config/";
    configFile="load-server.conf";
    lockFile="/tmp/load-server.lock";
    watchdogSocketName="/tmp/load-watchdog";
    email_invite_regular=configPath+"invitation_email_template_regular.html";
    email_invite_admin=configPath+"invitation_email_template_admin.html";
    email_revoke=configPath+"revoke_email_template.html";
    email_change_pass=configPath+"change_pass_email_template.html";
    email_delete_account=configPath+"delete_account_email_template.html";

    locale=QLocale("en_EN");

    //reports
    report_dir=mainPath+"reports/";
    report_template_path=configPath;
    report_html_file="report.html";
    report_pdf_file="report.pdf";
    report_doc_file="report.doc";
    report_map_template="map_template.tpl";
    //htmlToPdfCmd=QString("%1htmltopdf/wkhtmltopdf").arg(mainPath);
    htmlToPdfCmd=QString("wkhtmltopdf");

    exifToolCmd=QString("%1ExifTool/exiftool").arg(mainPath);
    report_domain="http://report.catchr.net";
    email_report=configPath+"email_report.html";
    email_report_www=configPath+"email_report_www.html";

    pdfs_dir = mainPath+"../pdfs/";
    invoices_dir = pdfs_dir;// + "invoices/";
    //templates_dir = mainPath+"reports/";

    // Payments
    //payPosId=QString::number(707069);
    payPosId=QString::number(146372);
    payApiLogin="146372";
    payApiPass="589a2912478f32f36e64206483bc49c8";
    payApiUrl=QString("https://sandbox.przelewy24.pl/api/v1"); //sandbox
    payTrnRegisterUrl=QString("https://sandbox.przelewy24.pl/api/v1/transaction/register"); //sandbox
    payTrnExecuteUrl=QString("https://sandbox.przelewy24.pl/trnRequest/");
    payTrnVerifyUrl=QString("https://sandbox.przelewy24.pl/api/v1/transaction/verify");
    payTrnRejectUrl=QString("https://sandbox.przelewy24.pl/api/v1/transaction/reject");
    //payCardUrl=QString("https://ssl.dotpay.pl/test_seller/api/v1/operations/");
    //payCardDeleteUrl=QString("https://ssl.dotpay.pl/test_payment/payment_api/v1/cards/");
    payRegisterOrderUrl=QString("localhost:3001/card/info/%1");//"https://sandbox.przelewy24.pl/api/v1/transaction/register");
    //payApiPIN=QString("589a2912478f32f36e64206483bc49c8");

    trnReturnUrl=QString("http://web.ns.fmdev.pl/");
    //trnReturnUrl=QString("file://index.html");

    payNotifyUrl = QString("http://94.172.190.163:8082/payment_status");
    payNotifyCardUrl = QString("http://94.172.190.163:8082/payment_card_status");

    payCRC=QString("4108f08a8a7b5ceb");

    SmsApiLogin = "webapi_load1";

    documentsPath=mainPath + "documents/";

    restApiIp="0.0.0.0";
    restApiPort=22111;
    //restApiPort=8082;
    webSessionDuration=60*60*2;
    webUseSsl=false;

    filePath=QString("%1%2").arg(mainPath).arg("files");

}
Singleton& Zm() {
// funkcja glowna singletonu
        static Singleton variables;
        return variables;
}
