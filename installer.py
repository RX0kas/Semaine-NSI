import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext
import os
import urllib.request
import json
import zipfile
import subprocess
import sys
import threading
from pathlib import Path
import re
import shutil
import webbrowser

# Constantes
ROOT = os.path.dirname(__file__)
GITHUB_REPO = "RX0kas/Semaine-NSI"
REPO_URL = f"https://api.github.com/repos/{GITHUB_REPO}"
LATEST_RELEASE_URL = f"{REPO_URL}/releases/latest"
RESOURCES_FOLDER = os.path.join(ROOT, "resources")
VENV_FOLDER = os.path.join(RESOURCES_FOLDER, ".venv")
INSTALLED_VERSION_FILE = "build.txt"

class ApplicationInstaller:
    def __init__(self, root):
        self.root = root
        self.root.title("Installateur Semaine-NSI")
        self.root.geometry("600x500")
        
        self.installed_version = None
        self.latest_version = None
        self.is_installed = False
        self.logs_visible = False
        self.release_notes = None
        self.releases = []  # historique complet


        self.setup_ui()
        self.check_installation()

    def fetch_releases_history(self):
        url = f"{REPO_URL}/releases"
        with urllib.request.urlopen(url) as response:
            self.releases = json.loads(response.read().decode())

        
    def setup_ui(self):
        title_label = ttk.Label(
            self.root, 
            text="Lanceur de l'application Semaine-NSI", 
            font=("Arial", 16, "bold")
        )
        title_label.pack(pady=20)
        
        status_frame = ttk.LabelFrame(self.root, text="Statut", padding=10)
        status_frame.pack(fill="x", padx=20, pady=10)
        
        self.status_label = ttk.Label(status_frame, text="Vérification de l'installation...")
        self.status_label.pack(anchor="w")
        
        self.version_label = ttk.Label(status_frame, text="Version : Non installée")
        self.version_label.pack(anchor="w")
        
        # Barre de progression
        progress_frame = ttk.LabelFrame(self.root, text="Progression", padding=10)
        progress_frame.pack(fill="x", padx=20, pady=10)
        
        self.progress_bar = ttk.Progressbar(
            progress_frame,
            mode="determinate",
            maximum=100
        )
        self.progress_bar.pack(fill="x", pady=5)
        self.progress_bar.pack_forget()  # cacher par défaut

        
        self.log_text = scrolledtext.ScrolledText(progress_frame, height=10)
        self.log_text.pack(fill="both", expand=True)
        self.log_text.pack_forget()
        self.log_text.config(state=tk.DISABLED)
        
        self.toggle_logs_button = ttk.Button(
            progress_frame,
            text="Afficher plus",
            command=self.toggle_logs
        )
        self.toggle_logs_button.pack(anchor="e", pady=(5, 0))

        buttons_frame = ttk.Frame(self.root)
        buttons_frame.pack(pady=20)

        self.install_button = ttk.Button(
            buttons_frame, 
            text="Installer / Mettre à jour", 
            command=self.start_installation,
            state=tk.DISABLED
        )
        self.install_button.pack(fill="x", pady=4)
        self.install_button.pack_forget()  # caché par défaut

        self.launch_button = ttk.Button(
            buttons_frame, 
            text="Lancer l'application", 
            command=self.launch_application,
            state=tk.DISABLED
        )
        self.launch_button.pack(fill="x", pady=4)

        self.history_button = ttk.Button(
            buttons_frame,
            text="Historique des versions",
            command=self.show_release_history
        )
        self.history_button.pack(fill="x", pady=4)

        self.release_notes_button = ttk.Button(
            buttons_frame,
            text="Notes de version",
            command=self.show_release_notes
        )
        self.release_notes_button.pack(fill="x", pady=4)

        self.refresh_button = ttk.Button(
            buttons_frame,
            text="Actualiser le statut",
            command=self.refresh_status
        )
        self.refresh_button.pack(fill="x", pady=4)

    def show_progress_bar(self):
        self.progress_bar.pack(fill="x", pady=5)

    def hide_progress_bar(self):
        self.progress_bar.pack_forget()
        self.set_progress(0)


    def toggle_logs(self):
        if self.logs_visible:
            self.log_text.pack_forget()
            self.toggle_logs_button.config(text="Afficher plus")
            self.logs_visible = False
        else:
            self.log_text.pack(fill="both", expand=True)
            self.toggle_logs_button.config(text="Masquer")
            self.logs_visible = True

    def log_message(self, message):
        # Always write logs internally; only show when log area visible
        self.log_text.config(state=tk.NORMAL)
        self.log_text.insert(tk.END, message + "\n")
        self.log_text.see(tk.END)
        self.log_text.config(state=tk.DISABLED)
        self.root.update()

    def set_progress(self, value):
        self.progress_bar["value"] = value
        self.root.update_idletasks()
        
    def check_installation(self):
        self.log_message("Vérification de l'installation actuelle...")
        
        if os.path.exists(RESOURCES_FOLDER):
            # Vérifie le fichier build
            version_file = os.path.join(RESOURCES_FOLDER, INSTALLED_VERSION_FILE)
            if os.path.exists(version_file):
                with open(version_file, 'r') as f:
                    self.installed_version = f.read().strip()
                self.is_installed = True
        
        # Vérifie si le fichier main existe
        main_py_path = os.path.join(RESOURCES_FOLDER, "src", "main.py")
        if os.path.exists(main_py_path):
            self.is_installed = True
            if not self.installed_version:
                self.installed_version = "Inconnue"
        
        venv_exists = os.path.exists(VENV_FOLDER)
        
        # Mise à jour de l'interface
        if self.is_installed and venv_exists:
            self.status_label.config(text="Statut : Installée et prête")
            self.version_label.config(text=f"Version : {self.installed_version or 'Inconnue'}")
            self.launch_button.config(state=tk.NORMAL)
            self.log_message("L'application est installée et prête à être lancée.")
        elif self.is_installed and not venv_exists:
            self.status_label.config(text="Statut : Installée (environnement virtuel manquant)")
            self.version_label.config(text=f"Version : {self.installed_version or 'Inconnue'}")
            self.log_message(
                "L'application est installée mais l'environnement virtuel est manquant."
            )
            self.root.after(500, self.ask_reinstall_venv)
        else:
            self.status_label.config(text="Statut : Non installée")
            self.version_label.config(text="Version : Non installée")
            self.log_message("L'application n'est pas installée.")
        
        self.get_latest_version()
        self.fetch_releases_history()


    def ask_reinstall_venv(self):
        answer = messagebox.askyesno(
            "Environnement virtuel manquant",
            "L'application est installée mais l'environnement virtuel est absent.\n\n"
            "Voulez-vous le recréer maintenant ?"
        )

        if answer:
            self.install_button.config(state=tk.DISABLED)
            self.launch_button.config(state=tk.DISABLED)

            thread = threading.Thread(target=self.reinstall_venv)
            thread.daemon = True
            thread.start()

    def reinstall_venv(self):
        self.log_message("Recréation de l'environnement virtuel...")

        try:
            if os.path.exists(VENV_FOLDER):
                shutil.rmtree(VENV_FOLDER)

            subprocess.run([sys.executable, "-m", "venv", VENV_FOLDER], check=True)

            requirements_path = os.path.join(RESOURCES_FOLDER, "requirements.txt")
            if os.path.exists(requirements_path):
                self.log_message("Installation des dépendances...")
                self.set_progress(85)
                if os.name == 'nt':
                    pip_path = os.path.join(VENV_FOLDER, "Scripts", "pip")
                else:
                    pip_path = os.path.join(VENV_FOLDER, "bin", "pip")

                subprocess.run([pip_path, "install", "-r", requirements_path], check=True)
                self.log_message("Dépendances installées avec succès.")
            else:
                self.log_message("Aucun requirements.txt trouvé.")

            self.log_message("Environnement virtuel recréé avec succès.")
            self.root.after(0, self.refresh_status)

        except Exception as e:
            if not self.logs_visible:
                self.toggle_logs()
            self.log_message(f"Erreur lors de la recréation du venv : {str(e)}")
            self.root.after(
                0, lambda: messagebox.showerror("Erreur", str(e))
            )

    def get_latest_version(self):
        try:
            self.log_message("Récupération des informations de la dernière version...")
            with urllib.request.urlopen(LATEST_RELEASE_URL) as response:
                data = json.loads(response.read().decode())
                self.latest_version = data['tag_name']
                self.release_notes = data.get("body", "Aucune note de version disponible.")
                
                # Majeur.Mineur.Correctif
                if re.match(r'^\d+\.\d+\.\d+$', self.latest_version):
                    self.log_message(f"Dernière version disponible : {self.latest_version}")
                    
                    if self.installed_version and self.is_update_needed():
                        self.status_label.config(text="Statut : Mise à jour disponible")
                        self.version_label.config(
                            text=f"Installée : {self.installed_version} | Dernière : {self.latest_version}"
                        )
                        self.install_button.config(state=tk.NORMAL, text="Mettre à jour")
                        self.install_button.pack(fill="x", pady=4)
                        self.log_message(
                            f"Mise à jour disponible : {self.installed_version} → {self.latest_version}"
                        )
                    elif not self.is_installed:
                        self.install_button.config(state=tk.NORMAL, text="Installer")
                        self.install_button.pack(fill="x", pady=4)
                        self.log_message("Prêt à installer la dernière version.")
                    elif self.is_installed and not os.path.exists(VENV_FOLDER):
                        self.install_button.config(text="Réparer", state=tk.NORMAL)
                        self.install_button.pack(fill="x", pady=4)

                    else:
                        self.install_button.pack_forget()
                        self.log_message("Vous disposez déjà de la dernière version.")
                else:
                    self.log_message(
                        "Attention : le tag de version ne respecte pas le format attendu x.x.x"
                    )
                    self.install_button.config(
                        state=tk.NORMAL if not self.is_installed else tk.DISABLED
                    )
                    
        except Exception as e:
            if not self.logs_visible:
                self.toggle_logs()
            self.log_message(f"Erreur lors de la récupération de la version : {str(e)}")
            self.latest_version = None
            self.install_button.config(
                state=tk.NORMAL if not self.is_installed else tk.DISABLED
            )

    def show_release_notes(self):
        window = tk.Toplevel(self.root)
        window.title(f"Notes de version – {self.latest_version}")
        window.geometry("600x400")
        window.transient(self.root)
        window.grab_set()

        text = scrolledtext.ScrolledText(window, wrap=tk.WORD)
        text.pack(fill="both", expand=True, padx=10, pady=10)

        text.insert(tk.END, self.release_notes if self.release_notes else "Aucune note de version disponible")
        text.config(state=tk.DISABLED)

        ttk.Button(window, text="Fermer", command=window.destroy).pack(pady=5)

    def open_on_github(self):
        if not self.latest_version:
            messagebox.showinfo("Info", "Aucune version connue pour ouvrir la page GitHub.")
            return
        url = f"https://github.com/{GITHUB_REPO}/releases/tag/{self.latest_version}"
        webbrowser.open(url)

    def show_release_history(self):
        window = tk.Toplevel(self.root)
        window.title("Historique des versions")
        window.geometry("750x450")
        window.transient(self.root)
        window.grab_set()

        left = ttk.Frame(window)
        left.pack(side=tk.LEFT, fill=tk.Y, padx=10, pady=10)

        right = ttk.Frame(window)
        right.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=10, pady=10)

        versions_list = tk.Listbox(left, width=25)
        versions_list.pack(fill=tk.Y, expand=True)

        notes_text = scrolledtext.ScrolledText(right, wrap=tk.WORD)
        notes_text.pack(fill=tk.BOTH, expand=True)
        notes_text.config(state=tk.DISABLED)

        install_btn = ttk.Button(
            right,
            text="Installer cette version",
            state=tk.DISABLED
        )
        install_btn.pack(pady=5)
        


        # remplir la liste
        for release in self.releases:
            versions_list.insert(tk.END, release["tag_name"])

        def on_select(event):
            index = versions_list.curselection()
            if not index:
                return

            release = self.releases[index[0]]
            notes = release.get("body", "Aucune note de version.")

            notes_text.config(state=tk.NORMAL)
            notes_text.delete(1.0, tk.END)
            notes_text.insert(tk.END, notes)
            notes_text.config(state=tk.DISABLED)

            install_btn.config(state=tk.NORMAL)

            install_btn.config(
                command=lambda: self.install_specific_release(release, window)
            )

        versions_list.bind("<<ListboxSelect>>", on_select)

    def install_specific_release(self, release, parent_window):
        parent_window.destroy()

        zip_url = None
        for asset in release.get("assets", []):
            if asset["name"] == "release.zip":
                zip_url = asset["browser_download_url"]
                break

        if not zip_url:
            messagebox.showerror(
                "Erreur",
                "Cette release ne contient pas de fichier release.zip"
            )
            return

        self.selected_release_url = zip_url
        self.selected_release_version = release["tag_name"]

        self.install_button.config(state=tk.DISABLED)
        self.launch_button.config(state=tk.DISABLED)

        thread = threading.Thread(
            target=self.install_application,
            args=(zip_url, self.selected_release_version)
        )
        thread.daemon = True
        thread.start()


    def is_update_needed(self):
        if not self.installed_version or not self.latest_version:
            return False
        
        try:
            installed_parts = list(map(int, self.installed_version.split('.')))
            latest_parts = list(map(int, self.latest_version.split('.')))
            
            for i in range(3):
                if latest_parts[i] > installed_parts[i]:
                    return True
                elif latest_parts[i] < installed_parts[i]:
                    return False
            return False  # Même version
        except:
            return False
    
    def start_installation(self):
        # Démarre l'installation dans un autre thread pour garder l'interface réactive
        self.install_button.config(state=tk.DISABLED)
        self.launch_button.config(state=tk.DISABLED)
        
        thread = threading.Thread(target=self.install_application)
        thread.daemon = True
        thread.start()
    
    def install_application(self, zip_url=None, forced_version=None):
        self.show_progress_bar()
        self.set_progress(0)

        self.log_message("Démarrage du processus d'installation...")
        

        zip_path = os.path.join(os.path.dirname(__file__), "release.zip")

        try:
            if zip_url is None:
                self.log_message("Récupération des informations de la dernière version...")
                with urllib.request.urlopen(LATEST_RELEASE_URL) as response:
                    release_data = json.loads(response.read().decode())
                    version = release_data["tag_name"]

                    zip_url = None
                    for asset in release_data.get("assets", []):
                        if asset["name"] == "release.zip":
                            zip_url = asset["browser_download_url"]
                            break

                    if not zip_url:
                        raise FileNotFoundError(
                            "Aucun fichier release.zip trouvé dans la dernière release"
                        )
            else:
                version = forced_version
                self.log_message(f"Installation de la version sélectionnée : {version}")

            os.makedirs(RESOURCES_FOLDER, exist_ok=True)

            self.log_message(f"Téléchargement de la version {version}...")
            with urllib.request.urlopen(zip_url) as response, open(zip_path, "wb") as out:
                total_size = int(response.headers.get("Content-Length", 0))
                downloaded = 0
                block_size = 8192

                while True:
                    chunk = response.read(block_size)
                    if not chunk:
                        break
                    out.write(chunk)
                    downloaded += len(chunk)

                    if total_size > 0:
                        self.set_progress((downloaded / total_size) * 50)

            self.log_message("Téléchargement terminé.")

            self.log_message("Nettoyage des anciens fichiers...")
            self.set_progress(55)

            for item in os.listdir(RESOURCES_FOLDER):
                if item != INSTALLED_VERSION_FILE:
                    path = os.path.join(RESOURCES_FOLDER, item)
                    if os.path.isdir(path):
                        shutil.rmtree(path)
                    else:
                        os.remove(path)

            self.log_message("Extraction des fichiers...")
            self.set_progress(65)

            with zipfile.ZipFile(zip_path, "r") as zip_ref:
                zip_ref.extractall(RESOURCES_FOLDER)

            with open(os.path.join(RESOURCES_FOLDER, INSTALLED_VERSION_FILE), "w") as f:
                f.write(version)

            self.installed_version = version
            self.log_message(f"Version {version} installée avec succès.")

            self.log_message("Création de l'environnement virtuel...")
            self.set_progress(75)

            if os.path.exists(VENV_FOLDER):
                shutil.rmtree(VENV_FOLDER)

            subprocess.run([sys.executable, "-m", "venv", VENV_FOLDER], check=True)

            requirements_path = os.path.join(RESOURCES_FOLDER, "requirements.txt")
            if os.path.exists(requirements_path):
                self.log_message("Installation des dépendances...")
                self.set_progress(85)

                pip_path = (
                    os.path.join(VENV_FOLDER, "Scripts", "pip")
                    if os.name == "nt"
                    else os.path.join(VENV_FOLDER, "bin", "pip")
                )

                subprocess.run(
                    [pip_path, "install", "-r", requirements_path],
                    check=True
                )
                self.log_message("Dépendances installées.")
            else:
                self.log_message("Aucun fichier requirements.txt trouvé.")

            self.set_progress(100)
            self.is_installed = True
            self.log_message("Installation terminée avec succès.")
            self.root.after(0, self.update_ui_after_install)

        except Exception as e:
            if not self.logs_visible:
                self.toggle_logs()
            self.log_message(f"Échec de l'installation : {e}")
            self.root.after(
                0, lambda: messagebox.showerror("Erreur d'installation", str(e))
            )
            self.root.after(0, self.refresh_status)

        finally:
            try:
                if os.path.exists(zip_path):
                    os.remove(zip_path)
                    self.log_message("Fichier release.zip supprimé.")
            except Exception as e:
                self.log_message(f"Impossible de supprimer release.zip : {e}")
            self.hide_progress_bar()
            self.refresh_status()


    
    def update_ui_after_install(self):
        self.status_label.config(text="Statut : Installée et prête")
        self.version_label.config(text=f"Version : {self.installed_version}")
        self.launch_button.config(state=tk.NORMAL)
        self.install_button.config(state=tk.DISABLED)
        messagebox.showinfo("Succès", "Application installée avec succès !")
    
    def launch_application(self):
        try:
            self.log_message("Lancement de l'application...")
            
            python_path = os.path.join(".venv", "Scripts", "python")
            
            main_py_path = os.path.join(RESOURCES_FOLDER, "src", "main.py")
            
            if not os.path.exists(main_py_path):
                raise FileNotFoundError(f"main.py introuvable : {main_py_path}")
            
            os.chdir(RESOURCES_FOLDER)
            
            cmd = [python_path, "-m", "src.main"]
            self.log_message(f"Exécution : {' '.join(cmd)}")
            
            subprocess.Popen(cmd, cwd=RESOURCES_FOLDER)
            
            self.log_message("Application lancée avec succès !")
            
        except Exception as e:
            if not self.logs_visible:
                self.toggle_logs()
            self.log_message(f"Échec du lancement de l'application : {str(e)}")
            messagebox.showerror("Erreur de lancement", str(e))
    
    def refresh_status(self):
        self.log_text.config(state=tk.NORMAL)
        self.log_text.delete(1.0, tk.END)
        self.log_text.config(state=tk.DISABLED)
        self.install_button.config(state=tk.DISABLED)
        self.launch_button.config(state=tk.DISABLED)
        self.check_installation()
        self.set_progress(0)

if __name__ == "__main__":
    root = tk.Tk()
    app = ApplicationInstaller(root)
    root.mainloop()