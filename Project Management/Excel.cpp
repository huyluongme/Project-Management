#include "Excel.h"

void cExcel::Import(const wchar_t* filename) {
	task.clear();
	libxl::Book* book = xlCreateXMLBook();
	if (book->load(filename)) {
		libxl::Sheet* sheet = book->getSheet(0);
		if (sheet) {
			for (int row = sheet->firstRow() + 1; row < sheet->lastRow(); ++row) {
				Task tmp;
				if (sheet->readStr(row, 0) != NULL)
					tmp.task = sheet->readStr(row, 0);
				if (sheet->readStr(row, 1) != NULL)
					tmp.descr = sheet->readStr(row, 1);
				tmp.dur = sheet->readNum(row, 2);
				if (sheet->readStr(row, 3) != NULL) {
					tmp.pre = sheet->readStr(row, 3);
					if (tmp.pre.find(L",") != std::wstring::npos) {
						std::wstring t = tmp.pre;
						int count = 0;
						size_t found = t.find(L",");
						while (found != std::wstring::npos) {
							count++;
							t.erase(0, found+1);
							found = t.find(L",");
						}
						tmp.deg_in = count + 1;
					}
					else
						tmp.deg_in = 1;
				}
				else
					tmp.deg_in = 0;
				tmp.deg_out = 0;
				tmp.rank = 0;
				tmp.critical = false;
				task.push_back(tmp);
			}
		}
	}
	book->release();
}

void cExcel::Export_Ex(const wchar_t* filename) {
	libxl::Book* book = xlCreateXMLBook();
	if (book) {
		libxl::Sheet * sheet = book->addSheet(L"Sheet1");
		if (sheet)
		{
			sheet->writeStr(1, 0, L"Task");
			sheet->writeStr(1, 1, L"Describe");
			sheet->writeStr(1, 2, L"Duration");
			sheet->writeStr(1, 2, L"Predecessors");
		}
		book->save(filename);
		book->release();
	}
}