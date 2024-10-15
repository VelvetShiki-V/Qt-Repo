
import logging
import logging.handlers
import datetime
import os

LOG_FILENAME = 'logging.out'

LEVELS = {
    'debug': logging.DEBUG,
    'info': logging.INFO,
    'warning': logging.WARNING,
    'error': logging.ERROR,
    'critical': logging.CRITICAL
}

logger = None


def WriteLog(level, line):
    global logger
    if logger and logger.hasHandlers():
        {
            'debug': logger.debug,
            'info': logger.info,
            'warning': logger.warning,
            'error': logger.error,
            'critical': logger.critical
        }[level](line)


def MakeLine(content):
    return "{} {}|{}".format(datetime.datetime.now(), os.getpid(), content)

def PLine(content):
    global logger
    if logger and logger.hasHandlers():
        logger.info(content)

def DLine(content):
    WriteLog('debug', MakeLine(content))

def ILine(content):
    WriteLog('info', MakeLine(content))

def WLine(content):
    WriteLog('warning', MakeLine(content))

def ELine(content):
    WriteLog('error', MakeLine(content))

def CLine(content):
    WriteLog('critical', MakeLine(content))


def start(log_level='info'):
    if log_level:
        global logger
        logger = logging.getLogger('AppServer')
        rf_handler = logging.handlers.RotatingFileHandler(
            filename=LOG_FILENAME,
            mode='a',
            maxBytes=10485760,
            backupCount=7,
            encoding=None,
            delay=0
        )
        logger.addHandler(rf_handler)
        logger.setLevel(LEVELS[log_level])


def alter(log_level):
    global logger
    logger.setLevel(LEVELS[log_level])
