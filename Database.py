from telegram import Update
from telegram.ext import Application, CommandHandler, CallbackContext
import pyodbc
import logging

async def waarde(update: Update, context: CallbackContext) -> None:
    weight = "{:.2f}". format(get_latest_weight_from_db())
    if weight is not None:
        await update.message.reply_text(f'De laatste gewichtwaarde is: {weight} g')
    else:
        await update.message.reply_text('Geen gegevens beschikbaar.')

def main():
    # Voer je eigen token in die je van BotFather hebt gekregen
    application = Application.builder().token("6998324646:AAFoh6_mfoINcOzhpMPf9ZNMbT95-CY5YGA").build()
    application.add_handler(CommandHandler("Waarde", waarde))

    # Start de Bot
    application.run_polling()

def get_db_connection():
    try:
        conn = pyodbc.connect(
            r'DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\Users\User\OneDrive\Documenten\Database1.accdb;')
        return conn
    except pyodbc.Error as e:
        logging.error(f"Error connecting to database: {e}")
        return None

# De laatste gewichtwaarde uit de Access database ophalen
def get_latest_weight_from_db():
    conn = get_db_connection()
    if conn:
        try:
            cursor = conn.cursor()
            cursor.execute('SELECT TOP 1 gewicht FROM [waardes sensor] ORDER BY ID DESC')
            row = cursor.fetchone()
            cursor.close()
            conn.close()
            if row:
                return row[0]
        except pyodbc.Error as e:
            logging.error(f"Error reading from database: {e}")
    return None

if __name__ == '__main__':
    main()
