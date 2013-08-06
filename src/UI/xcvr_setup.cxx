//======================================================================
//
// Xcvr setup dialog
//
// A part of flrig
//
// Copyright 2012, David Freese, W1HKJ@w1hkj.com
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  It is
// copyright under the GNU General Public License.
//
// You should have received a copy of the GNU General Public License
// along with the program; if not, write to the Free Software
// Foundation, Inc.
// 59 Temple Place, Suite 330
// Boston, MA  02111-1307 USA
//
//======================================================================

Fl_Tabs *tabsConfig=(Fl_Tabs *)0;
Fl_Group *tabPrimary=(Fl_Group *)0;
Fl_ComboBox *selectRig=(Fl_ComboBox *)0;
Fl_Counter *cntRigCatRetries=(Fl_Counter *)0;
Fl_Counter *cntRigCatTimeout=(Fl_Counter *)0;
Fl_Counter *cntRigCatWait=(Fl_Counter *)0;
Fl_Counter *query_interval=(Fl_Counter *)0;
Fl_ComboBox *selectCommPort=(Fl_ComboBox *)0;
Fl_ComboBox *mnuBaudrate=(Fl_ComboBox *)0;
Fl_Check_Button *btnTwoStopBit=(Fl_Check_Button *)0;
Fl_Check_Button *btnOneStopBit=(Fl_Check_Button *)0;
Fl_Check_Button *btnRigCatEcho=(Fl_Check_Button *)0;
Fl_Input *server_addr=(Fl_Input *)0;
Fl_Int_Input *server_port=(Fl_Int_Input *)0;
Fl_Round_Button *btncatptt=(Fl_Round_Button *)0;
Fl_Round_Button *btnrtsptt=(Fl_Round_Button *)0;
Fl_Round_Button *btndtrptt=(Fl_Round_Button *)0;
Fl_Check_Button *chkrtscts=(Fl_Check_Button *)0;
Fl_Check_Button *btnrtsplus=(Fl_Check_Button *)0;
Fl_Check_Button *btndtrplus=(Fl_Check_Button *)0;
Fl_Int_Input *txtCIV=(Fl_Int_Input *)0;
Fl_Button *btnCIVdefault=(Fl_Button *)0;
Fl_Check_Button *btnUSBaudio=(Fl_Check_Button *)0;
Fl_Check_Button *btn_key_fldigi=(Fl_Check_Button *)0;
Fl_Group *tabPTT=(Fl_Group *)0;
Fl_ComboBox *selectSepPTTPort=(Fl_ComboBox *)0;
Fl_Round_Button *btnSepRTSptt=(Fl_Round_Button *)0;
Fl_Check_Button *btnSepRTSplus=(Fl_Check_Button *)0;
Fl_Round_Button *btnSepDTRptt=(Fl_Round_Button *)0;
Fl_Check_Button *btnSepDTRplus=(Fl_Check_Button *)0;
Fl_Group *tabAux=(Fl_Group *)0;
Fl_ComboBox *selectAuxPort=(Fl_ComboBox *)0;
Fl_Group *tabPolling=(Fl_Group *)0;
Fl_Value_Input *poll_smeter=(Fl_Value_Input *)0;
Fl_Value_Input *poll_pout=(Fl_Value_Input *)0;
Fl_Value_Input *poll_swr=(Fl_Value_Input *)0;
Fl_Value_Input *poll_alc=(Fl_Value_Input *)0;
Fl_Value_Input *poll_frequency=(Fl_Value_Input *)0;
Fl_Value_Input *poll_mode=(Fl_Value_Input *)0;
Fl_Value_Input *poll_bandwidth=(Fl_Value_Input *)0;
Fl_Value_Input *poll_volume=(Fl_Value_Input *)0;
Fl_Value_Input *poll_micgain=(Fl_Value_Input *)0;
Fl_Value_Input *poll_rfgain=(Fl_Value_Input *)0;
Fl_Value_Input *poll_power_control=(Fl_Value_Input *)0;
Fl_Value_Input *poll_ifshift=(Fl_Value_Input *)0;
Fl_Value_Input *poll_notch=(Fl_Value_Input *)0;
Fl_Value_Input *poll_auto_notch=(Fl_Value_Input *)0;
Fl_Value_Input *poll_pre_att=(Fl_Value_Input *)0;
Fl_Value_Input *poll_squelch=(Fl_Value_Input *)0;
Fl_Value_Input *poll_split=(Fl_Value_Input *)0;
Fl_Value_Input *poll_noise=(Fl_Value_Input *)0;
Fl_Value_Input *poll_nr=(Fl_Value_Input *)0;
Fl_Button *btnClearAddControls=(Fl_Button *)0;
Fl_Value_Input *poll_all=(Fl_Value_Input *)0;
Fl_Button *btnSetAllAdd=(Fl_Button *)0;
Fl_Group *tabSndCmd=(Fl_Group *)0;
Fl_Input2 *txt_command=(Fl_Input2 *)0;
Fl_Button *btn_send_command=(Fl_Button *)0;
Fl_Output *txt_response=(Fl_Output *)0;
Fl_Box *box_xcvr_connect=(Fl_Box *)0;
Fl_Box *box_fldigi_connect=(Fl_Box *)0;
Fl_Button *btnCancelCommConfig=(Fl_Button *)0;
Fl_Return_Button *btnOkXcvrDialog=(Fl_Return_Button *)0;

static void cb_selectRig(Fl_ComboBox*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
	initConfigDialog();
}

static void cb_cntRigCatRetries(Fl_Counter* o, void*) {
	progStatus.comm_retries = (int)o->value();
}

static void cb_cntRigCatTimeout(Fl_Counter* o, void*) {
	progStatus.comm_timeout = (int)o->value();
}

static void cb_cntRigCatWait(Fl_Counter* o, void*) {
	progStatus.comm_wait = (int)o->value();
}

static void cb_query_interval(Fl_Counter* o, void*) {
	progStatus.serloop_timing = (int)o->value();
}

static void cb_selectCommPort(Fl_ComboBox*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_mnuBaudrate(Fl_ComboBox*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btnTwoStopBit(Fl_Check_Button* o, void*) {
	if (o->value() == true) {
		btnOneStopBit->value(false);
		progStatus.stopbits = 2;
	} else
		o->value(true);
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btnOneStopBit(Fl_Check_Button* o, void*) {
	if (o->value() == true) {
		btnTwoStopBit->value(false);
		progStatus.stopbits = 1;
	} else
		o->value(true);
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btnRigCatEcho(Fl_Check_Button*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_server_addr(Fl_Input* o, void*) {
	progStatus.server_addr = o->value();
}

static void cb_server_port(Fl_Int_Input* o, void*) {
	progStatus.server_port = o->value();
}

static void cb_btncatptt(Fl_Round_Button* o, void*) {
	if (o->value()== 1) {
		btnrtsptt->value(0);
		btndtrptt->value(0);
	}
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btnrtsptt(Fl_Round_Button* o, void*) {
	if (o->value() == 1)
		btncatptt->value(0);
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btndtrptt(Fl_Round_Button* o, void*) {
	if (o->value() == 1)
		btncatptt->value(0);
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_chkrtscts(Fl_Check_Button*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btnrtsplus(Fl_Check_Button*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btndtrplus(Fl_Check_Button*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_txtCIV(Fl_Int_Input* o, void*) {
	progStatus.CIV = atol(o->value());
	cbCIV();
}

static void cb_btnCIVdefault(Fl_Button*, void*) {
	cbCIVdefault();
}

static void cb_btnUSBaudio(Fl_Check_Button*, void*) {
	cbUSBaudio();
}

static void cb_btn_key_fldigi(Fl_Check_Button* o, void*) {
	progStatus.key_fldigi = o->value();
}

static void cb_selectSepPTTPort(Fl_ComboBox*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btnSepRTSptt(Fl_Round_Button* o, void*) {
	if (o->value() == 1)
		btnSepDTRptt->value(0);
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btnSepRTSplus(Fl_Check_Button*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btnSepDTRptt(Fl_Round_Button* o, void*) {
	if (o->value() == 1)
		btnSepRTSptt->value(0);
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_btnSepDTRplus(Fl_Check_Button*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_selectAuxPort(Fl_ComboBox*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw();
}

static void cb_poll_smeter(Fl_Value_Input* o, void*) {
	progStatus.poll_smeter = o->value();
}

static void cb_poll_pout(Fl_Value_Input* o, void*) {
	progStatus.poll_pout = o->value();
}

static void cb_poll_swr(Fl_Value_Input* o, void*) {
	progStatus.poll_swr = o->value();
}

static void cb_poll_alc(Fl_Value_Input* o, void*) {
	progStatus.poll_alc = o->value();
}

static void cb_poll_frequency(Fl_Value_Input* o, void*) {
	progStatus.poll_frequency = o->value();
}

static void cb_poll_mode(Fl_Value_Input* o, void*) {
	progStatus.poll_mode = o->value();
}

static void cb_poll_bandwidth(Fl_Value_Input* o, void*) {
	progStatus.poll_bandwidth = o->value();
}

static void cb_poll_volume(Fl_Value_Input* o, void*) {
	progStatus.poll_volume = o->value();
}

static void cb_poll_micgain(Fl_Value_Input* o, void*) {
	progStatus.poll_micgain = o->value();
}

static void cb_poll_rfgain(Fl_Value_Input* o, void*) {
	progStatus.poll_rfgain = o->value();
}

static void cb_poll_power_control(Fl_Value_Input* o, void*) {
	progStatus.poll_power_control = o->value();
}

static void cb_poll_ifshift(Fl_Value_Input* o, void*) {
	progStatus.poll_ifshift = o->value();
}

static void cb_poll_notch(Fl_Value_Input* o, void*) {
	progStatus.poll_notch = o->value();
}

static void cb_poll_auto_notch(Fl_Value_Input* o, void*) {
	progStatus.poll_auto_notch = o->value();
}

static void cb_poll_pre_att(Fl_Value_Input* o, void*) {
	progStatus.poll_pre_att = o->value();
}

static void cb_poll_squelch(Fl_Value_Input* o, void*) {
	progStatus.poll_squelch = o->value();
}

static void cb_poll_split(Fl_Value_Input* o, void*) {
	progStatus.poll_split = o->value();
}

static void cb_poll_noise(Fl_Value_Input* o, void*) {
	progStatus.poll_noise = o->value();
}

static void cb_poll_nr(Fl_Value_Input* o, void*) {
	progStatus.poll_noise = o->value();
}

static void cb_btnClearAddControls(Fl_Button*, void*) {
	poll_volume->value(0);
	poll_micgain->value(0);
	poll_rfgain->value(0);
	poll_power_control->value(0);
	poll_ifshift->value(0);
	poll_notch->value(0);
	poll_auto_notch->value(0);
	poll_pre_att->value(0);
	poll_squelch->value(0);
	poll_split->value(0);
	poll_noise->value(0);
	poll_nr->value(0);

	progStatus.poll_volume = 0;
	progStatus.poll_micgain = 0;
	progStatus.poll_rfgain = 0;
	progStatus.poll_power_control = 0;
	progStatus.poll_ifshift = 0;
	progStatus.poll_notch = 0;
	progStatus.poll_auto_notch = 0;
	progStatus.poll_pre_att = 0;
	progStatus.poll_squelch = 0;
	progStatus.poll_split = 0;
	progStatus.poll_noise = 0;
	progStatus.poll_nr = 0;
}

static void cb_poll_all(Fl_Value_Input* o, void*) {
	progStatus.poll_all = o->value();
}

static void cb_btnSetAllAdd(Fl_Button*, void*) {
	poll_volume->value(progStatus.poll_all);
	poll_micgain->value(progStatus.poll_all);
	poll_rfgain->value(progStatus.poll_all);
	poll_power_control->value(progStatus.poll_all);
	poll_ifshift->value(progStatus.poll_all);
	poll_notch->value(progStatus.poll_all);
	poll_auto_notch->value(progStatus.poll_all);
	poll_pre_att->value(progStatus.poll_all);
	poll_squelch->value(progStatus.poll_all);
	poll_split->value(progStatus.poll_all);
	poll_noise->value(progStatus.poll_all);
	poll_nr->value(progStatus.poll_all);

	progStatus.poll_volume = progStatus.poll_all;
	progStatus.poll_micgain = progStatus.poll_all;
	progStatus.poll_rfgain = progStatus.poll_all;
	progStatus.poll_power_control = progStatus.poll_all;
	progStatus.poll_ifshift = progStatus.poll_all;
	progStatus.poll_notch = progStatus.poll_all;
	progStatus.poll_auto_notch = progStatus.poll_all;
	progStatus.poll_pre_att = progStatus.poll_all;
	progStatus.poll_squelch = progStatus.poll_all;
	progStatus.poll_split = progStatus.poll_all;
	progStatus.poll_noise = progStatus.poll_all;
	progStatus.poll_nr = progStatus.poll_all;
}

static void cb_btn_send_command(Fl_Button*, void*) {
	cb_send_command();
}

static void cb_btnCancelCommConfig(Fl_Button*, void*) {
	cbCancelXcvrDialog();
}

static void cb_btnOkXcvrDialog(Fl_Return_Button*, void*) {
	cbOkXcvrDialog();
}

Fl_Double_Window* XcvrDialog() {

Fl_Double_Window* w = new Fl_Double_Window(480, 255, _("I/O Ports"));

tabsConfig = new Fl_Tabs(0, 8, 482, 246);

	tabPrimary = new Fl_Group(2, 30, 475, 222, _("Primary"));

		Fl_Group* xcr_grp1 = new Fl_Group(5, 34, 195, 140);
			xcr_grp1->box(FL_ENGRAVED_FRAME);

			selectRig = new Fl_ComboBox(41, 38, 155, 22, _("Rig:"));
			selectRig->tooltip(_("Select Transceiver"));
			selectRig->box(FL_DOWN_BOX);
			selectRig->color(FL_BACKGROUND2_COLOR);
			selectRig->selection_color(FL_BACKGROUND_COLOR);
			selectRig->labeltype(FL_NORMAL_LABEL);
			selectRig->labelfont(0);
			selectRig->labelsize(14);
			selectRig->labelcolor(FL_FOREGROUND_COLOR);
			selectRig->callback((Fl_Callback*)cb_selectRig);
			selectRig->align(Fl_Align(FL_ALIGN_LEFT));
			selectRig->when(FL_WHEN_RELEASE);
			selectRig->end();

			cntRigCatRetries = new Fl_Counter(96, 63, 100, 22, _("Retries"));
			cntRigCatRetries->tooltip(_("Number of  times to resend\ncommand before giving up"));
			cntRigCatRetries->minimum(1);
			cntRigCatRetries->maximum(10);
			cntRigCatRetries->step(1);
			cntRigCatRetries->value(5);
			cntRigCatRetries->callback((Fl_Callback*)cb_cntRigCatRetries);
			cntRigCatRetries->align(Fl_Align(FL_ALIGN_LEFT));
			cntRigCatRetries->value(progStatus.comm_retries);
			cntRigCatRetries->lstep(10);

			cntRigCatTimeout = new Fl_Counter(96, 89, 100, 22, _("Retry intvl"));
			cntRigCatTimeout->tooltip(_("Time between retries is msec"));
			cntRigCatTimeout->minimum(2);
			cntRigCatTimeout->maximum(200);
			cntRigCatTimeout->step(1);
			cntRigCatTimeout->value(10);
			cntRigCatTimeout->callback((Fl_Callback*)cb_cntRigCatTimeout);
			cntRigCatTimeout->align(Fl_Align(FL_ALIGN_LEFT));
			cntRigCatTimeout->value(progStatus.comm_timeout);
			cntRigCatTimeout->lstep(10);

			cntRigCatWait = new Fl_Counter(96, 115, 100, 22, _("Cmds"));
			cntRigCatWait->tooltip(_("Wait millseconds between sequential commands"));
			cntRigCatWait->minimum(0);
			cntRigCatWait->maximum(100);
			cntRigCatWait->step(1);
			cntRigCatWait->value(5);
			cntRigCatWait->callback((Fl_Callback*)cb_cntRigCatWait);
			cntRigCatWait->align(Fl_Align(FL_ALIGN_LEFT));
			cntRigCatWait->value(progStatus.comm_wait);
			cntRigCatWait->lstep(10);

			query_interval = new Fl_Counter(96, 141, 100, 22, _("Poll intvl"));
			query_interval->tooltip(_("Polling interval in msec"));
			query_interval->minimum(10);
			query_interval->maximum(5000);
			query_interval->step(1);
			query_interval->value(50);
			query_interval->callback((Fl_Callback*)cb_query_interval);
			query_interval->align(Fl_Align(FL_ALIGN_LEFT));
			query_interval->value(progStatus.serloop_timing);
			query_interval->lstep(10);

		xcr_grp1->end();

		Fl_Group* xcr_grp2 = new Fl_Group(202, 94, 270, 80);
			xcr_grp2->tooltip(_("Two stop bits"));
			xcr_grp2->box(FL_ENGRAVED_FRAME);

			selectCommPort = new Fl_ComboBox(278, 99, 190, 22, _("Ser. Port"));
			selectCommPort->tooltip(_("Xcvr serial port"));
			selectCommPort->box(FL_DOWN_BOX);
			selectCommPort->color(FL_BACKGROUND2_COLOR);
			selectCommPort->selection_color(FL_BACKGROUND_COLOR);
			selectCommPort->labeltype(FL_NORMAL_LABEL);
			selectCommPort->labelfont(0);
			selectCommPort->labelsize(14);
			selectCommPort->labelcolor(FL_FOREGROUND_COLOR);
			selectCommPort->callback((Fl_Callback*)cb_selectCommPort);
			selectCommPort->align(Fl_Align(FL_ALIGN_LEFT));
			selectCommPort->when(FL_WHEN_RELEASE);
			selectCommPort->end();

			mnuBaudrate = new Fl_ComboBox(278, 125, 190, 22, _("Baud:"));
			mnuBaudrate->tooltip(_("Xcvr baudrate"));
			mnuBaudrate->box(FL_DOWN_BOX);
			mnuBaudrate->color(FL_BACKGROUND2_COLOR);
			mnuBaudrate->selection_color(FL_BACKGROUND_COLOR);
			mnuBaudrate->labeltype(FL_NORMAL_LABEL);
			mnuBaudrate->labelfont(0);
			mnuBaudrate->labelsize(14);
			mnuBaudrate->labelcolor(FL_FOREGROUND_COLOR);
			mnuBaudrate->callback((Fl_Callback*)cb_mnuBaudrate);
			mnuBaudrate->align(Fl_Align(FL_ALIGN_LEFT));
			mnuBaudrate->when(FL_WHEN_RELEASE);
			mnuBaudrate->end();

			btnTwoStopBit = new Fl_Check_Button(250, 152, 22, 15, _("2 -StopBits"));
			btnTwoStopBit->down_box(FL_DOWN_BOX);
			btnTwoStopBit->callback((Fl_Callback*)cb_btnTwoStopBit);
			btnTwoStopBit->align(Fl_Align(FL_ALIGN_RIGHT));
			btnTwoStopBit->value(progStatus.stopbits == 2);

			btnOneStopBit = new Fl_Check_Button(210, 152, 22, 15, _("1"));
			btnOneStopBit->tooltip(_("One Stop Bit"));
			btnOneStopBit->down_box(FL_DOWN_BOX);
			btnOneStopBit->callback((Fl_Callback*)cb_btnOneStopBit);
			btnOneStopBit->align(Fl_Align(FL_ALIGN_RIGHT));
			btnOneStopBit->value(progStatus.stopbits == 1);

			btnRigCatEcho = new Fl_Check_Button(397, 152, 22, 15, _("Echo "));
			btnRigCatEcho->down_box(FL_DOWN_BOX);
			btnRigCatEcho->callback((Fl_Callback*)cb_btnRigCatEcho);
			btnRigCatEcho->align(Fl_Align(FL_ALIGN_RIGHT));
			btnRigCatEcho->value(progStatus.comm_echo);

		xcr_grp2->end();
        
		Fl_Group* xcr_grp3 = new Fl_Group(202, 34, 270, 64);
			xcr_grp3->box(FL_ENGRAVED_FRAME);

			server_addr = new Fl_Input(317, 41, 140, 22, _("Fldigi address:"));
			server_addr->tooltip(_("xmlrpc server address (7362)\nchange requires restart\nAre you sure?"));
			server_addr->callback((Fl_Callback*)cb_server_addr);
			server_addr->value(progStatus.server_addr.c_str());

			server_port = new Fl_Int_Input(317, 67, 100, 22, _("Fldigi port:"));
			server_port->tooltip(_("xmlrpc server address (7362)\nchange requires restart\nAre you sure?"));
			server_port->type(2);
			server_port->callback((Fl_Callback*)cb_server_port);
			server_port->value(progStatus.server_port.c_str());

		xcr_grp3->end();
        
        
		Fl_Group* xcr_grp4 = new Fl_Group(4, 175, 243, 73);
			xcr_grp4->box(FL_ENGRAVED_FRAME);

			btncatptt = new Fl_Round_Button(15, 180, 149, 22, _("PTT via CAT"));
			btncatptt->tooltip(_("PTT is a CAT command (not hardware)"));
			btncatptt->down_box(FL_ROUND_DOWN_BOX);
			btncatptt->callback((Fl_Callback*)cb_btncatptt);
			btncatptt->value(progStatus.comm_catptt);

			btnrtsptt = new Fl_Round_Button(15, 203, 149, 22, _("PTT via RTS"));
			btnrtsptt->tooltip(_("RTS is ptt line"));
			btnrtsptt->down_box(FL_ROUND_DOWN_BOX);
			btnrtsptt->callback((Fl_Callback*)cb_btnrtsptt);
			btnrtsptt->value(progStatus.comm_rtsptt);

			btndtrptt = new Fl_Round_Button(15, 226, 149, 22, _("PTT via DTR"));
			btndtrptt->tooltip(_("DTR is ptt line"));
			btndtrptt->down_box(FL_ROUND_DOWN_BOX);
			btndtrptt->callback((Fl_Callback*)cb_btndtrptt);
			btndtrptt->value(progStatus.comm_dtrptt);

			chkrtscts = new Fl_Check_Button(129, 178, 98, 21, _("RTS/CTS"));
			chkrtscts->tooltip(_("Xcvr uses RTS/CTS handshake"));
			chkrtscts->down_box(FL_DOWN_BOX);
			chkrtscts->callback((Fl_Callback*)cb_chkrtscts);
			chkrtscts->value(progStatus.comm_rtscts);

			btnrtsplus = new Fl_Check_Button(129, 201, 102, 21, _("RTS +12 v"));
			btnrtsplus->tooltip(_("Initial state of RTS"));
			btnrtsplus->down_box(FL_DOWN_BOX);
			btnrtsplus->callback((Fl_Callback*)cb_btnrtsplus);
			btnrtsplus->value(progStatus.comm_rtsplus);

			btndtrplus = new Fl_Check_Button(129, 224, 100, 21, _("DTR +12 v"));
			btndtrplus->tooltip(_("Initial state of DTR"));
			btndtrplus->down_box(FL_DOWN_BOX);
			btndtrplus->callback((Fl_Callback*)cb_btndtrplus);
			btndtrplus->value(progStatus.comm_dtrplus);

		xcr_grp4->end();
        
		Fl_Group* xcr_grp5 = new Fl_Group(247, 175, 225, 36);
			xcr_grp5->box(FL_ENGRAVED_FRAME);
			xcr_grp5->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

			txtCIV = new Fl_Int_Input(261, 183, 58, 22, _("CI-V adr"));
			txtCIV->tooltip(_("Enter hex value, ie: 0x5F"));
			txtCIV->type(2);
			txtCIV->callback((Fl_Callback*)cb_txtCIV);
			txtCIV->align(Fl_Align(FL_ALIGN_RIGHT));

			btnCIVdefault = new Fl_Button(387, 183, 69, 22, _("Default"));
			btnCIVdefault->callback((Fl_Callback*)cb_btnCIVdefault);

		xcr_grp5->end();
       
		Fl_Group* xcr_grp6 = new Fl_Group(247, 212, 225, 36);
			xcr_grp6->box(FL_ENGRAVED_FRAME);

			btnUSBaudio = new Fl_Check_Button(261, 222, 104, 15, _("USB audio"));
			btnUSBaudio->down_box(FL_DOWN_BOX);
			btnUSBaudio->callback((Fl_Callback*)cb_btnUSBaudio);

			btn_key_fldigi = new Fl_Check_Button(375, 222, 85, 15, _("key fldigi"));
			btn_key_fldigi->tooltip(_("PTT keys fldigi modem"));
			btn_key_fldigi->down_box(FL_DOWN_BOX);
			btn_key_fldigi->callback((Fl_Callback*)cb_btn_key_fldigi);
			btn_key_fldigi->value(progStatus.key_fldigi);

		xcr_grp6->end();

	tabPrimary->end();

	tabPTT = new Fl_Group(2, 30, 475, 222, _("Sep\' PTT"));
		tabPTT->hide();

		Fl_Box *bxptt = new Fl_Box(53, 73, 399, 37, 
_("Use only if your setup requires a separate\nSerial Port for a PTT control line"));
		bxptt->box(FL_FLAT_BOX);

		selectSepPTTPort = new Fl_ComboBox(128, 121, 192, 22, _("PTT Port"));
		selectSepPTTPort->tooltip(_("Aux control port"));
		selectSepPTTPort->box(FL_DOWN_BOX);
		selectSepPTTPort->color(FL_BACKGROUND2_COLOR);
		selectSepPTTPort->selection_color(FL_BACKGROUND_COLOR);
		selectSepPTTPort->labeltype(FL_NORMAL_LABEL);
		selectSepPTTPort->labelfont(0);
		selectSepPTTPort->labelsize(14);
		selectSepPTTPort->labelcolor(FL_FOREGROUND_COLOR);
		selectSepPTTPort->callback((Fl_Callback*)cb_selectSepPTTPort);
		selectSepPTTPort->align(Fl_Align(FL_ALIGN_LEFT));
		selectSepPTTPort->when(FL_WHEN_RELEASE);
		selectSepPTTPort->end();

		btnSepRTSptt = new Fl_Round_Button(111, 159, 129, 23, _("PTT via RTS"));
		btnSepRTSptt->tooltip(_("RTS is ptt line"));
		btnSepRTSptt->down_box(FL_ROUND_DOWN_BOX);
		btnSepRTSptt->callback((Fl_Callback*)cb_btnSepRTSptt);
		btnSepRTSptt->value(progStatus.sep_rtsptt);

		btnSepRTSplus = new Fl_Check_Button(274, 159, 128, 23, _("RTS +12 v"));
		btnSepRTSplus->tooltip(_("Initial state of RTS"));
		btnSepRTSplus->down_box(FL_DOWN_BOX);
		btnSepRTSplus->callback((Fl_Callback*)cb_btnSepRTSplus);
		btnSepRTSplus->value(progStatus.sep_rtsplus);

		btnSepDTRptt = new Fl_Round_Button(111, 186, 129, 22, _("PTT via DTR"));
		btnSepDTRptt->tooltip(_("DTR is ptt line"));
		btnSepDTRptt->down_box(FL_ROUND_DOWN_BOX);
		btnSepDTRptt->callback((Fl_Callback*)cb_btnSepDTRptt);
		btnSepDTRptt->value(progStatus.sep_dtrptt);

		btnSepDTRplus = new Fl_Check_Button(274, 186, 128, 22, _("DTR +12 v"));
		btnSepDTRplus->tooltip(_("Initial state of DTR"));
		btnSepDTRplus->down_box(FL_DOWN_BOX);
		btnSepDTRplus->callback((Fl_Callback*)cb_btnSepDTRplus);
		btnSepDTRplus->value(progStatus.sep_dtrplus);

	tabPTT->end();


	tabAux = new Fl_Group(2, 30, 475, 222, _("Aux"));
		tabAux->hide();
        
		selectAuxPort = new Fl_ComboBox(131, 132, 192, 22, _("Aux Port"));
		selectAuxPort->tooltip(_("Aux control port"));
		selectAuxPort->box(FL_DOWN_BOX);
		selectAuxPort->color(FL_BACKGROUND2_COLOR);
		selectAuxPort->selection_color(FL_BACKGROUND_COLOR);
		selectAuxPort->labeltype(FL_NORMAL_LABEL);
		selectAuxPort->labelfont(0);
		selectAuxPort->labelsize(14);
		selectAuxPort->labelcolor(FL_FOREGROUND_COLOR);
		selectAuxPort->callback((Fl_Callback*)cb_selectAuxPort);
		selectAuxPort->align(Fl_Align(FL_ALIGN_LEFT));
		selectAuxPort->when(FL_WHEN_RELEASE);
		selectAuxPort->end();

		Fl_Box *bxsep = new Fl_Box(56, 84, 399, 38, 
_("Use only if your setup requires a separate\nSerial Port for a special Control Signals"));
		bxsep->box(FL_FLAT_BOX);

	tabAux->end();


	tabPolling = new Fl_Group(2, 30, 476, 222, _("Polling"));
		tabPolling->hide();

		Fl_Group* xcr_grp7 = new Fl_Group(4, 34, 474, 48, _("Meters"));
			xcr_grp7->box(FL_ENGRAVED_BOX);
			xcr_grp7->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

			poll_smeter = new Fl_Value_Input(13, 55, 30, 20, _("S-meter"));
			poll_smeter->tooltip(_("Poll every Nth interval"));
			poll_smeter->maximum(10);
			poll_smeter->step(1);
			poll_smeter->value(1);
			poll_smeter->callback((Fl_Callback*)cb_poll_smeter);
			poll_smeter->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_smeter->value(progStatus.poll_smeter);

			poll_pout = new Fl_Value_Input(132, 55, 30, 20, _("Power out"));
			poll_pout->tooltip(_("Poll every Nth interval"));
			poll_pout->maximum(10);
			poll_pout->step(1);
			poll_pout->value(1);
			poll_pout->callback((Fl_Callback*)cb_poll_pout);
			poll_pout->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_pout->value(progStatus.poll_pout);

			poll_swr = new Fl_Value_Input(252, 55, 30, 20, _("SWR"));
			poll_swr->tooltip(_("Poll every Nth interval"));
			poll_swr->maximum(10);
			poll_swr->step(1);
			poll_swr->value(1);
			poll_swr->callback((Fl_Callback*)cb_poll_swr);
			poll_swr->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_swr->value(progStatus.poll_swr);

			poll_alc = new Fl_Value_Input(372, 55, 30, 20, _("ALC"));
			poll_alc->tooltip(_("Poll every Nth interval"));
			poll_alc->maximum(10);
			poll_alc->step(1);
			poll_alc->value(1);
			poll_alc->callback((Fl_Callback*)cb_poll_alc);
			poll_alc->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_alc->value(progStatus.poll_alc);

		xcr_grp7->end();


		Fl_Group* xcr_grp8 = new Fl_Group(4, 82, 474, 48, _("Operating Controls"));
			xcr_grp8->box(FL_ENGRAVED_BOX);
			xcr_grp8->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

			poll_frequency = new Fl_Value_Input(13, 103, 30, 20, _("Frequency"));
			poll_frequency->tooltip(_("Poll every Nth interval"));
			poll_frequency->maximum(10);
			poll_frequency->step(1);
			poll_frequency->value(1);
			poll_frequency->callback((Fl_Callback*)cb_poll_frequency);
			poll_frequency->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_frequency->value(progStatus.poll_frequency);

			poll_mode = new Fl_Value_Input(132, 103, 30, 20, _("Mode"));
			poll_mode->tooltip(_("Poll every Nth interval"));
			poll_mode->maximum(10);
			poll_mode->step(1);
			poll_mode->value(1);
			poll_mode->callback((Fl_Callback*)cb_poll_mode);
			poll_mode->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_mode->value(progStatus.poll_mode);

			poll_bandwidth = new Fl_Value_Input(252, 103, 30, 20, _("Bandwidth"));
			poll_bandwidth->tooltip(_("Poll every Nth interval"));
			poll_bandwidth->maximum(10);
			poll_bandwidth->step(1);
			poll_bandwidth->value(1);
			poll_bandwidth->callback((Fl_Callback*)cb_poll_bandwidth);
			poll_bandwidth->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_bandwidth->value(progStatus.poll_bandwidth);

		xcr_grp8->end();

		Fl_Group* xcr_grp9 = new Fl_Group(4, 130, 474, 122, _("Additional Controls"));
			xcr_grp9->box(FL_ENGRAVED_FRAME);
			xcr_grp9->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

			poll_volume = new Fl_Value_Input(12, 150, 30, 20, _("Volume"));
			poll_volume->tooltip(_("Poll every Nth interval"));
			poll_volume->maximum(10);
			poll_volume->step(1);
			poll_volume->callback((Fl_Callback*)cb_poll_volume);
			poll_volume->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_volume->value(progStatus.poll_volume);

			poll_micgain = new Fl_Value_Input(131, 150, 30, 20, _("Mic gain"));
			poll_micgain->tooltip(_("Poll every Nth interval"));
			poll_micgain->maximum(10);
			poll_micgain->step(1);
			poll_micgain->callback((Fl_Callback*)cb_poll_micgain);
			poll_micgain->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_micgain->value(progStatus.poll_micgain);

			poll_rfgain = new Fl_Value_Input(251, 150, 30, 20, _("RF gain"));
			poll_rfgain->tooltip(_("Poll every Nth interval"));
			poll_rfgain->maximum(10);
			poll_rfgain->step(1);
			poll_rfgain->callback((Fl_Callback*)cb_poll_rfgain);
			poll_rfgain->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_rfgain->value(progStatus.poll_rfgain);

			poll_power_control = new Fl_Value_Input(371, 150, 30, 20, _("Power"));
			poll_power_control->tooltip(_("Poll every Nth interval"));
			poll_power_control->maximum(10);
			poll_power_control->step(1);
			poll_power_control->callback((Fl_Callback*)cb_poll_power_control);
			poll_power_control->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_power_control->value(progStatus.poll_power_control);

			poll_ifshift = new Fl_Value_Input(12, 175, 30, 20, _("IF shift"));
			poll_ifshift->tooltip(_("Poll every Nth interval"));
			poll_ifshift->maximum(10);
			poll_ifshift->step(1);
			poll_ifshift->callback((Fl_Callback*)cb_poll_ifshift);
			poll_ifshift->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_ifshift->value(progStatus.poll_ifshift);

			poll_notch = new Fl_Value_Input(131, 175, 30, 20, _("Man\' Notch"));
			poll_notch->tooltip(_("Poll every Nth interval"));
			poll_notch->maximum(10);
			poll_notch->step(1);
			poll_notch->callback((Fl_Callback*)cb_poll_notch);
			poll_notch->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_notch->value(progStatus.poll_notch);

			poll_auto_notch = new Fl_Value_Input(251, 175, 30, 20, _("Auto notch"));
			poll_auto_notch->tooltip(_("Poll every Nth interval"));
			poll_auto_notch->maximum(10);
			poll_auto_notch->step(1);
			poll_auto_notch->callback((Fl_Callback*)cb_poll_auto_notch);
			poll_auto_notch->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_auto_notch->value(progStatus.poll_auto_notch);

			poll_pre_att = new Fl_Value_Input(12, 200, 30, 20, _("Pre/Att"));
			poll_pre_att->tooltip(_("Poll every Nth interval"));
			poll_pre_att->maximum(10);
			poll_pre_att->step(1);
			poll_pre_att->callback((Fl_Callback*)cb_poll_pre_att);
			poll_pre_att->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_pre_att->value(progStatus.poll_pre_att);

			poll_squelch = new Fl_Value_Input(131, 200, 30, 20, _("Squelch"));
			poll_squelch->tooltip(_("Poll every Nth interval"));
			poll_squelch->maximum(10);
			poll_squelch->step(1);
			poll_squelch->callback((Fl_Callback*)cb_poll_squelch);
			poll_squelch->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_squelch->value(progStatus.poll_squelch);

			poll_split = new Fl_Value_Input(251, 200, 30, 20, _("Split"));
			poll_split->tooltip(_("Poll every Nth interval"));
			poll_split->maximum(10);
			poll_split->step(1);
			poll_split->callback((Fl_Callback*)cb_poll_split);
			poll_split->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_split->value(progStatus.poll_split);

			poll_noise = new Fl_Value_Input(13, 225, 30, 20, _("Noise"));
			poll_noise->tooltip(_("Poll every Nth interval"));
			poll_noise->maximum(10);
			poll_noise->step(1);
			poll_noise->callback((Fl_Callback*)cb_poll_noise);
			poll_noise->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_noise->value(progStatus.poll_noise);

			poll_nr = new Fl_Value_Input(132, 225, 30, 20, _("Noise red\'"));
			poll_nr->tooltip(_("Poll every Nth interval"));
			poll_nr->maximum(10);
			poll_nr->step(1);
			poll_nr->callback((Fl_Callback*)cb_poll_nr);
			poll_nr->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_nr->value(progStatus.poll_noise);

			btnClearAddControls = new Fl_Button(330, 200, 70, 20, _("Clear all"));
			btnClearAddControls->callback((Fl_Callback*)cb_btnClearAddControls);

			poll_all = new Fl_Value_Input(408, 225, 30, 20);
			poll_all->tooltip(_("Poll every Nth interval"));
			poll_all->maximum(10);
			poll_all->step(1);
			poll_all->value(4);
			poll_all->callback((Fl_Callback*)cb_poll_all);
			poll_all->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
			poll_all->value(progStatus.poll_all);

			btnSetAllAdd = new Fl_Button(331, 225, 70, 20, _("Set all to"));
			btnSetAllAdd->callback((Fl_Callback*)cb_btnSetAllAdd);

		xcr_grp9->end();

	tabPolling->end();

	tabSndCmd = new Fl_Group(2, 30, 475, 222, _("Send Cmd"));
		tabSndCmd->hide();

		txt_command = new Fl_Input2(29, 53, 434, 23, 
_("Enter text as ASCII string\nOr sequence of hex values, x80 etc separated by spaces"));
		txt_command->box(FL_DOWN_BOX);
		txt_command->color(FL_BACKGROUND2_COLOR);
		txt_command->selection_color(FL_SELECTION_COLOR);
		txt_command->labeltype(FL_NORMAL_LABEL);
		txt_command->labelfont(0);
		txt_command->labelsize(14);
		txt_command->labelcolor(FL_FOREGROUND_COLOR);
		txt_command->align(Fl_Align(FL_ALIGN_BOTTOM_LEFT));
		txt_command->when(FL_WHEN_RELEASE);

		btn_send_command = new Fl_Button(392, 156, 71, 19, _("SEND"));
		btn_send_command->callback((Fl_Callback*)cb_btn_send_command);

		txt_response = new Fl_Output(29, 115, 434, 23, _("Response to the SEND button"));
		txt_response->align(Fl_Align(FL_ALIGN_BOTTOM_LEFT));

		box_xcvr_connect = new Fl_Box(29, 180, 18, 18, _("Connected to transceiver"));
		box_xcvr_connect->tooltip(_("Lit when connected"));
		box_xcvr_connect->box(FL_DIAMOND_DOWN_BOX);
		box_xcvr_connect->color(FL_LIGHT1);
		box_xcvr_connect->align(Fl_Align(FL_ALIGN_RIGHT));

		box_fldigi_connect = new Fl_Box(29, 204, 18, 18, _("Connected to fldigi"));
		box_fldigi_connect->tooltip(_("Lit when connected"));
		box_fldigi_connect->box(FL_DIAMOND_DOWN_BOX);
		box_fldigi_connect->color(FL_LIGHT1);
		box_fldigi_connect->align(Fl_Align(FL_ALIGN_RIGHT));

	tabSndCmd->end();

tabsConfig->end();
  
btnCancelCommConfig = new Fl_Button(346, 2, 60, 25, _("Close"));
btnCancelCommConfig->callback((Fl_Callback*)cb_btnCancelCommConfig);

btnOkXcvrDialog = new Fl_Return_Button(416, 2, 60, 25, _("Init"));
btnOkXcvrDialog->callback((Fl_Callback*)cb_btnOkXcvrDialog);

w->end();

return w;

}
