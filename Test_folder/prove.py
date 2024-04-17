import smtplib
from email.message import EmailMessage

# Set up email account information
email_address = "sender.uni@hotmail.com"
email_password = "Haloreach1!"
smtp_server = "smtp-mail.outlook.com"
smtp_port = 587

# Create email message
msg = EmailMessage()
msg['Subject'] = 'Re: Hello, World!'
msg['From'] = email_address
msg['To'] = 'michele.faeddal@hotmail.com'
msg.set_content('Hello Gilfoyle, this is a test email with an attachment. -Dinesh')
with open('test_results_optimized.xlsx', 'rb') as f:
    file_data = f.read()
msg.add_attachment(file_data, maintype='application', subtype='octet-stream', filename='test_results_optimized.xlsx')

# Send email message
try:
    with smtplib.SMTP(smtp_server, smtp_port) as server:
        server.starttls()
        server.login(email_address, email_password)
        server.send_message(msg)
    print("Email sent successfully!")
except Exception as e:
    print("An error occurred:", e)
