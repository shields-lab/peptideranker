from os.path import basename
from typing import List

import smtplib
from email.mime.application import MIMEApplication
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.utils import COMMASPACE, formatdate


def send_mail(send_to, subject, text,
              username: str, app_passwd: str,
              files: List[str] = None,
              domain: str = 'gmail.com') -> bool:
    """
    Sends an email with optional file attachments using Gmail's SMTP server.

    :param send_to: A single recipient or a list of recipient email addresses.
    :type send_to: str or List[str]

    :param subject: Subject line of the email.
    :type subject: str

    :param text: Plain text content of the email body.
    :type text: str

    :param username: Gmail account username without the domain (e.g., "@gmail.com").
    :type username: str

    :param app_passwd: App-specific password generated from Google Account settings. This is required instead of the regular account password when two-factor authentication is enabled.
    :type app_passwd: str

    :param files: List of file paths to attach to the email. Each path should point to a readable file on disk. If omitted or None, no files will be attached.
    :type files: List[str], optional

    :param domain: Email domain. Defaults to "gmail.com"
    :type domain: str, optional

    :return: Returns True if successful, False if not
    :rtype: bool

    :raises FileNotFoundError: If any file in the `files` list cannot be found.
    :raises smtplib.SMTPException: If an error occurs during the SMTP session.
    """
    if isinstance(send_to, str):
        send_to = [send_to]

    msg = MIMEMultipart()
    sender = f'{username}@{domain}'
    msg['From'] = sender
    msg['To'] = COMMASPACE.join(send_to)
    msg['Date'] = formatdate(localtime=True)
    msg['Subject'] = subject

    msg.attach(MIMEText(text))

    for f in files or []:
        with open(f, "rb") as fil:
            part = MIMEApplication(
                fil.read(),
                Name=basename(f)
            )
        # After the file is closed
        part['Content-Disposition'] = 'attachment; filename="%s"' % basename(f)
        msg.attach(part)

    smtp = smtplib.SMTP('smtp.gmail.com', 587)
    smtp.starttls()
    smtp.login(sender, app_passwd)
    smtp.sendmail(sender, send_to, msg.as_string())
    smtp.close()
