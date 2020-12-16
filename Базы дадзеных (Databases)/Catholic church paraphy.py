import PySimpleGUI as sg
import re
import sqlite3
from matplotlib.ticker import NullFormatter  # useful for `logit` scale
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import PySimpleGUI as sg
import matplotlib
matplotlib.use('TkAgg')

def draw_figure(canvas, figure):
    figure_canvas_agg = FigureCanvasTkAgg(figure, canvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='top', fill='both', expand=1)
    return figure_canvas_agg

def write(results):
    for item in results:
        for el in item:
            print(el, end='\t')
        print ("")
    print ("")

def select_info():
    cursor.execute(f"""SELECT Name 
                        FROM Member 
                        INNER JOIN Member_Unity
                            ON Member.Member_id=Member_Unity.Member_id
                        INNER JOIN Unity
                            ON Member_Unity.Unity_id=Unity.Unity_id
                            WHERE Member.SNLN = \'{values[0]}\';""")
    res_1 = cursor.fetchall()
    cursor.execute(f"""SELECT Name 
                        FROM Member 
                        INNER JOIN Member_Event
                            ON Member.Member_id=Member_Event.Member_id
                        INNER JOIN Event
                            ON Member_Event.Event_id=Event.Event_id
                            WHERE Member.SNLN = \'{values[0]}\';""")
    res_2 = cursor.fetchall()
    print("Unities of this man:\n")
    write(res_1)
    print("Events of this man:\n")
    write(res_2)

def max_unities():
    cursor.execute(f"""SELECT COUNT(1) FROM Unity;""")
    for item in cursor.fetchall():
        for el in item:
            return int(el)
    
def get_count_of_members():
    cursor.execute(f"""SELECT COUNT(Member_id) FROM Member_Unity GROUP BY Unity_id;""")
    res = cursor.fetchall()
    l = []
    for item in res:
        for el in item:
            l.append(el)
    return l

board = 220*"-"

conn = sqlite3.connect('Парафія.sqlite3')
cursor = conn.cursor()

fig = matplotlib.figure.Figure(figsize=(5, 4), dpi=100)
t = np.arange(0, max_unities(), 1)
fig.add_subplot(111).plot(t, get_count_of_members())

layout = [
    [sg.Text(board, font=("Helvetica", 4))],
    [sg.Text('Add member')],
    [sg.Text('SNLN:', size=(10, 1)), sg.InputText()],
    [sg.Text('Age:', size=(10, 1)), sg.InputText()],
    [sg.Text('Profession:', size=(10, 1)), sg.InputText()],
    [sg.Checkbox(text='Unity:', size=(7, 1)), sg.InputText()],
    [sg.Checkbox(text='Event:', size=(7, 1)), sg.InputText()],
    [sg.Button('Add')],
    [sg.Text(board, font=("Helvetica", 4))],
    [sg.Canvas(key='-CANVAS-'),
     sg.Output(size=(45, 20), key = '_output_')]
]
 
window = sg.Window('Catholic church paraphy', layout, finalize=True)

fig_canvas_agg = draw_figure(window['-CANVAS-'].TKCanvas, fig)

while True:                             # The Event Loop
    event, values = window.read()

    if event in (None, 'Exit', 'Cancel'):
        break

    if event == 'Add' and values[0] != "" and values[1] != "" and values[2] != "":
        window.FindElement('_output_').Update('')
        if values[3] is True and values[4] == "":
            print("You did't enter Unity")
        elif values[5] is True and values[6] == "":
            print("You did't enter Event")
        else:
            try:
                cursor.execute(f"SELECT * FROM Member WHERE SNLN = \'{values[0]}\';")   
                if len(cursor.fetchall()) != 0:
                    print("This man is existing.")
                    select_info()
                else:
                    cursor.execute(f"INSERT INTO Member (SNLN, Age, Profession) VALUES (\'{values[0]}\', \'{values[1]}\', \'{values[2]}\');")
                    conn.commit()
                    print("New man was added")

                    if values[3] is True:
                        cursor.execute(f"SELECT * FROM Unity WHERE Name = \'{values[4]}\';")
                        if len(cursor.fetchall()) == 0:
                            print("This unity doesn't exist in database")
                        else:
                            cursor.execute(f"SELECT Unity_id FROM Unity WHERE Name = \'{values[4]}\';")
                            res_1 = cursor.fetchall()
                            cursor.execute(f"SELECT Member_id FROM Member WHERE SNLN = \'{values[0]}\';")
                            res_2 = cursor.fetchall()
                            cursor.execute(f"INSERT INTO Member_Unity (Member_id, Unity_id, Position) VALUES (\'{list(res_2[0])[0]}\', \'{list(res_1[0])[0]}\', \'Удзельнік\');")
                            conn.commit()

                            # fig = matplotlib.figure.Figure(figsize=(5, 4), dpi=100)
                            # fig.add_subplot(111).plot(t, get_count_of_members())
                            # # window['-CANVAS-'].TKCanvas.clear()
                            # fig_canvas_agg = draw_figure(window['-CANVAS-'].TKCanvas, fig)
                            # window.Refresh()
                    if values[5] is True:
                        cursor.execute(f"SELECT * FROM Event WHERE Name = \'{values[6]}\';")
                        if len(cursor.fetchall()) == 0:
                            print("This event doesn't exist in database")
                        else:
                            cursor.execute(f"SELECT Event_id FROM Event WHERE Name = \'{values[6]}\';")
                            res_1 = cursor.fetchall()
                            cursor.execute(f"SELECT Member_id FROM Member WHERE SNLN = \'{values[0]}\';")
                            res_2 = cursor.fetchall()
                            cursor.execute(f"INSERT INTO Member_Event (Member_id, Event_id) VALUES (\'{list(res_2[0])[0]}\', \'{list(res_1[0])[0]}\');")
                            conn.commit()
            except:
                print("Error") 
    elif event == 'Add' and (values[0] == "" or values[1] == "" or values[2] == ""):
        window.FindElement('_output_').Update('')
        print("Enter SNLN, Age and Profession necessarily.")

conn.close()

window.close()