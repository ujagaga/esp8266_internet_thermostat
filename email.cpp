#include <EMailSender.h>
#include "email.h"

#define SMTP_PORT       465
#define SENDER_EMAIL    "SenderAddr@gmail.com"
#define SENDER_PASSWORD "yourAppPass"
#define SMTP_SERVER     "smtp.gmail.com"
#define RECIPIENT       "RecipientAddr@gmail.com"
#define SUBJECT         "Web Thermostat IP Change"



void EMAIL_send(String txtMessage){
  if(txtMessage.length() < 3){
    return;
  }
  
  EMailSender emailSend(SENDER_EMAIL, SENDER_PASSWORD);
  
  EMailSender::EMailMessage message;
  message.subject = SUBJECT;
  message.message = txtMessage;

  EMailSender::Response resp = emailSend.send(RECIPIENT, message);

  Serial.println("Sending status: ");
  Serial.println(resp.desc);
 
}
