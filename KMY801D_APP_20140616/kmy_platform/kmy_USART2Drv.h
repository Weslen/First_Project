#ifndef __KMY_USART2DRV_H
#define __KMY_USART2DRV_H




void kmy_USART2Init(unsigned long BaudRate);
//�������ܣ����ڳ�ʼ��
//����������brd: ������
//����ֵ����
//ע�������




void kmy_USART2SendString(const unsigned char *Buff);
//�������ܣ� �����ַ���
//����������
//	Buff: �����ַ�����buff
//����ֵ����
//ע�������




void kmy_USART2SendData(const unsigned char *Buff, unsigned int SendSize);
//�������ܣ�  ��������
//����������
//	Buff:  �������ݵ�buff
//	SendSize:�������ݵĳ���
//����ֵ����
//ע�������





void kmy_USART2ReceiveReset(void);
//�������ܣ� ��USART2�ѽ��ܵ������ֽڶ���
//������������
//����ֵ����
//ע�������




unsigned int kmy_USART2ReceiveData(unsigned char *Buff,unsigned int RecSize,unsigned long timeout_MilliSeconds);
//�������ܣ� ���ܴ��ڵ�����
//����������
//	Buff: �������ݴ�ŵĵط�
//	RecSize: ���ܶ��ٸ��ֽ�
//	timeout_MilliSeconds: ��ʱ�ĺ�����
//����ֵ�����ؽ��ܵ����ֽ���
//ע�������

#endif
