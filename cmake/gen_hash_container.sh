#!/usr/bin/env bash
HASH_SIZE_LIST=("27" "81" "243" "6561" "8019")
TYPE_LIST=("queue" "stack")

HASH_TEMPLATE_DIR="./utils/containers/hash"
for HASH_TYPE in ${TYPE_LIST[@]}; do
    for HASH_FILE in ${HASH_TEMPLATE_DIR}/hash_${HASH_TYPE}*.tpl; do
        FILE_NAME=$(basename "${HASH_FILE}" .tpl)
        for HASH_SIZE in ${HASH_SIZE_LIST[@]}; do
            HASH_FILE_NAME=${FILE_NAME/hash/hash${HASH_SIZE}}
            echo "cp ${HASH_FILE} ${HASH_TEMPLATE_DIR}/${HASH_FILE_NAME}"
            cp ${HASH_FILE} ${HASH_TEMPLATE_DIR}/${HASH_FILE_NAME}
            if [[ "$OSTYPE" == "darwin" ]]; then
                sed -i.bak "s/{SIZE}/${HASH_SIZE}/g" ${HASH_TEMPLATE_DIR}/${HASH_FILE_NAME}
            else
                sed -i "s/{SIZE}/${HASH_SIZE}/g" ${HASH_TEMPLATE_DIR}/${HASH_FILE_NAME}
            fi
        done
    done
done
