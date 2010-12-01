;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; File name: ` ~/.emacs '
;;; ---------------------
;;;
;;; Note: This file switches between two Emacs versions:
;;;            GNU-Emacs (19.34) and X-Emacs (19.14/20.X).
;;;       Please to not mix both versions: GNU-Emacs and X-Emacs
;;;       are incompatible. They use differnet binary code for
;;;       compiled lisp files and they have different builtin
;;;       lisp functions ... not only names of such functions
;;;       are different!!!
;;;
;;; If you need your own personal ~/.emacs
;;; please make a copy of this file
;;; an placein your changes and/or extension.
;;;
;;; Copyright (c) 1997 S.u.S.E. Gmbh Fuerth, Germany.  All rights reserved.
;;;
;;; Author: Werner Fink, <werner@suse.de> 1997
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Test of Emacs derivates
;;; -----------------------
(if (string-match "XEmacs\\|Lucid" emacs-version)
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;; XEmacs
  ;;; ------
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  (progn
      ;;
      ;; If not exists create the XEmacs options file
      ;; --------------------------------------------
      (if (and (not (file-readable-p "~/.xemacs-options"))
	       (fboundp 'save-options-menu-settings))
	(save-options-menu-settings))
      ;;
      ;; Remember font and more settings
      ;; -------------------------------
      (setq options-save-faces t)
      ;;
      ;; AUC-TeX
      ;; -------
      (if  (or (file-accessible-directory-p
	        "/usr/X11R6/lib/xemacs/site-lisp/auctex/")
       		(or (and (= emacs-major-version 19)
			 (>= emacs-minor-version 15))
           	    (= emacs-major-version 20)))
       (progn
	   (require 'tex-site)
	   (setq-default TeX-master nil)
	   ; Users private libaries 
	   ; (setq TeX-macro-private '("~/lib/tex-lib/"))
	   ;    AUC-TeX-Macros
	   ; (setq TeX-style-private   "~/lib/xemacs/site-lisp/auctex/style/")
	   ;    Autom. Auc-TeX-Macros
	   ; (setq TeX-auto-private    "~/lib/xemacs/site-lisp/auctex/auto/")
	))
      ;;
      ;; preload ispell
      ;; --------------
      (if (file-executable-p "/usr/bin/ispell") (require 'ispell)))
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;; GNU-Emacs
  ;;; ---------
  ;;; load ~/.gnu-emacs or, if not exists /etc/skel/.gnu-emacs
  ;;; For a description and the settings see /etc/skel/.gnu-emacs
  ;;;   ... for your private ~/.gnu-emacs your are at your one.
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  (if (file-readable-p "~/.gnu-emacs")
      (load "~/.gnu-emacs" nil t)
    (if (file-readable-p "/etc/skel/.gnu-emacs")
	(load "/etc/skel/.gnu-emacs" nil t)))
;;;
)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; XEmacs load options
;;; -------------------
;;; If missing the next few lines they will be append automatically
;;; by xemacs. This will be done by `save-options-menu-settings'
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Custum Settings
;; ===============
;; Set custom-file to ~/.xemacs-custom for XEmacs to avoid overlap with the
;; custom interface of GNU-Emacs. Nevertheless, in most cases GNU-Emacs can
;; not handle unknown functions in ~/.emacs .. therefore ~/.xemacs-custom.
(cond
 ((string-match "XEmacs" emacs-version)
	(setq custom-file "~/.xemacs-custom")
	(load "~/.xemacs-custom" t t)))
;; ======================
;; End of Custum Settings

;; Options Menu Settings
;; =====================
(cond
 ((and (string-match "XEmacs" emacs-version)
       (boundp 'emacs-major-version)
       (or (and
            (= emacs-major-version 19)
            (>= emacs-minor-version 14))
           (= emacs-major-version 20))
       (fboundp 'load-options-file))
  (load-options-file "~/.xemacs-options")))
;; ============================
;; End of Options Menu Settings
