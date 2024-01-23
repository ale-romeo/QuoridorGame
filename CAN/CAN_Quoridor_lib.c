#include "CAN.h"
#include "../GLCD/GLCD.h"

//Creazione di un intero a 32 bit contenente la codifica della mossa
uint32_t createMoveUint(Move* move) {
    uint32_t moveUint = 0;
    moveUint |= ((uint32_t)move->playerID << PLAYER_ID_SHIFT);
    moveUint |= ((uint32_t)move->moveType << MOVE_TYPE_SHIFT) & (MOVE_TYPE_MASK << MOVE_TYPE_SHIFT);
    moveUint |= ((uint32_t)move->wallOrientation << WALL_ORIENTATION_SHIFT) & (WALL_ORIENTATION_MASK << WALL_ORIENTATION_SHIFT);
    moveUint |= ((uint32_t)move->newXPos << NEW_X_POS_SHIFT) & (NEW_X_POS_MASK << NEW_X_POS_SHIFT);
    moveUint |= ((uint32_t)move->newYPos << NEW_Y_POS_SHIFT) & (NEW_Y_POS_MASK << NEW_Y_POS_SHIFT);
    return moveUint;
}

//Ottiene la mossa (Move) dall'intero a 32 bit codificato
Move getMoveFromUint(uint32_t moveUint) {
    Move move;
    move.playerID = (moveUint >> PLAYER_ID_SHIFT);
    move.moveType = (moveUint >> MOVE_TYPE_SHIFT) & MOVE_TYPE_MASK;
    move.wallOrientation = (moveUint >> WALL_ORIENTATION_SHIFT) & WALL_ORIENTATION_MASK;
    move.newXPos = (moveUint >> NEW_X_POS_SHIFT) & NEW_X_POS_MASK;
    move.newYPos = (moveUint >> NEW_Y_POS_SHIFT) & NEW_Y_POS_MASK;
    return move;
}

//Reset della mossa (Move)
void ResetMove(Move* move, unsigned int pID){
	move->playerID = (uint8_t) pID;
	move->moveType = (uint8_t) 0;
	move->wallOrientation = (uint8_t) 1;
	move->newXPos = (uint8_t) 0;
	move->newYPos = (uint8_t) 0;
}

//Invia ping tramite CAN2 con id 1
void SendPing(void){
	Move ping;
	uint32_t pingUint;
	ping.playerID = 0xFF;
	ping.moveType = 0;
	ping.wallOrientation = 0;
	ping.newXPos = (uint8_t) 0;
	ping.newYPos = (uint8_t) 0;
	pingUint = createMoveUint(&ping);
	CAN_TxMsg.data[0] = (pingUint >> 24) & 0xFF;
	CAN_TxMsg.data[1] = (pingUint >> 16) & 0xFF;
	CAN_TxMsg.data[2] = (pingUint >> 8) & 0xFF;
	CAN_TxMsg.data[3] = pingUint & 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 1;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (2, &CAN_TxMsg);               /* transmit message */
}

//Invia handshake di inizio partita tramite CAN2 con id 1
void SendHandshake(void){
	Move handshake;
	uint32_t handshakeUint;
	handshake.playerID = 0xAA;
	handshake.moveType = 0;
	handshake.wallOrientation = 0;
	handshake.newXPos = (uint8_t) 0;
	handshake.newYPos = (uint8_t) 0;
	handshakeUint = createMoveUint(&handshake);
	CAN_TxMsg.data[0] = (handshakeUint >> 24) & 0xFF;
	CAN_TxMsg.data[1] = (handshakeUint >> 16) & 0xFF;
	CAN_TxMsg.data[2] = (handshakeUint >> 8) & 0xFF;
	CAN_TxMsg.data[3] = handshakeUint & 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 1;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (2, &CAN_TxMsg);               /* transmit message */
}

//Invia la mossa tramite CAN2 con id 1
void SendMove(uint32_t moveUint) {
	CAN_TxMsg.data[0] = (moveUint >> 24) & 0xFF;
	CAN_TxMsg.data[1] = (moveUint >> 16) & 0xFF;
	CAN_TxMsg.data[2] = (moveUint >> 8) & 0xFF;
	CAN_TxMsg.data[3] = moveUint & 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 1;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (2, &CAN_TxMsg);               /* transmit message */
}
